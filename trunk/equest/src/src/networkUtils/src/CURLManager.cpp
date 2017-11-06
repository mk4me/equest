#include <networkUtils/CURLManager.h>
#include <mutex>
#include <condition_variable>
#include <map>
#include <set>
#include <utils/DummyWrap.h>
#include <utils/Debug.h>
#include <utils/SmartPtr.h>
#include <curl/multi.h>

using namespace networkUtils;
using namespace utils;

class CURLManager::CURLManagerImpl
{
private:
	//! ScopedLock
	using ScopedLock = std::lock_guard<std::mutex>;
	//! Zbiór uchwytów
	using CURLsSet = std::set<CURLPtr>;
	//! Mapa uchwytów i obiektów na których czekamy
	using CURLsWaitMap = std::map<CURLPtr, std::promise<CURLcode>>;

private:

	void unlockAndRemove(CURLsWaitMap::iterator it, CURLcode curlCode)
	{
		std::promise<CURLcode> locPromise(std::move(it->second));
		currentCurls.erase(it);
		locPromise.set_value(curlCode);
	}

	void innerRemove()
	{
		for (auto & curl : toRemoveCurlsSet){
			auto IT = currentCurls.find(curl);
			const auto ret = curl_multi_remove_handle(multi.get(), curl.get());
			if (ret == CURLM_OK){			
				manager->onCancelRequest(curl);			
			}
			else{
				std::stringstream ss;
				ss << "Internal multi curl error code: " << ret;
				manager->onErrorRequest(curl, ss.str());
			}

			unlockAndRemove(IT, CURLE_ABORTED_BY_CALLBACK);
		}

		//czuszczê kolejkê do usuniêcia
		CURLsSet().swap(toRemoveCurlsSet);
	}

	bool changeToContinue() const
	{
		return toAddCurlsSet.empty() == false
			|| currentCurls.empty() == false
			|| toRemoveCurlsSet.empty() == false
			|| finalize_ == true;
	}

public:
	CURLManagerImpl(CURLMPtr multi, CURLManager * manager) : multi(multi),
		manager(manager), finalize_(false), finalized_(false)
	{
		if (multi == nullptr){
			throw std::runtime_error("Invalid multi handle");
		}

		if (manager == nullptr){
			throw std::runtime_error("Invalid manager");
		}
	}

	~CURLManagerImpl()
	{
		
	}

	void finalize()
	{
		ScopedLock lock(sync);
		finalize_ = true;
		condVar.notify_one();
	}

	void run()
	{
		while (finalized_ == false){
			try{
				process();
			}
			catch (...){

			}
		}
	}

	std::future<CURLcode> addRequest(CURLPtr curl)
	{
		ScopedLock lock(sync);

		if (finalize_ == true){
			throw std::runtime_error("Manager already finalized");
		}

		// czy jest ju¿ dodany do obs³ugi?
		if (currentCurls.find(curl) != currentCurls.end() ||
			toAddCurlsSet.find(curl) != toAddCurlsSet.end()){

			throw std::runtime_error("CURL handle already managed");
		}
		
		// usuwam jeli by³ w kolejce do usuniêcia
		toRemoveCurlsSet.erase(curl);
		//dodajemy do kolejki
		auto f = toAddCurlsSet[curl].get_future();
		// budzimy przetwarzanie
		condVar.notify_one();
		//zwracamy przez przeniesienie
		return std::move(f);
	}

	void removeRequest(CURLPtr curl)
	{
		ScopedLock lock(sync);

		if (finalize_ == true){
			throw std::runtime_error("Manager already finalized");
		}

		// czy aktualnie przetwarzany
		auto it = currentCurls.find(curl);
		if (it == currentCurls.end()){
			// czy w kolejce do dowania
			auto IT = toAddCurlsSet.find(curl);
			if(IT != toAddCurlsSet.end()){
				manager->onRemoveRequest(curl);
				IT->second.set_value(CURLE_OK);
				toAddCurlsSet.erase(IT);
			}
		}
		else{
			// do usuniecia - anulowania
			toRemoveCurlsSet.insert(curl);
		}
		
		condVar.notify_one();
	}

	void updateHandles()
	{
		if (toRemoveCurlsSet.empty() == false || toAddCurlsSet.empty() == false)
		{
			ScopedLock lock(sync);
			if (toRemoveCurlsSet.empty() == false || toAddCurlsSet.empty() == false)
			{
				//najpierw próbujemy usuwaæ
				innerRemove();

				if (finalize_ == false){

					//teraz próbujemy dodawaæ
					for (auto it = toAddCurlsSet.begin(); it != toAddCurlsSet.end(); ++it){
						auto curl = it->first;
						if (curl_multi_add_handle(multi.get(), curl.get()) == CURLM_OK){
							manager->onAddRequest(curl);
							currentCurls.insert(std::move(*it));
						}
					}

					//czuszczê kolejkê do dodania
					CURLsWaitMap().swap(toAddCurlsSet);
				}
			}
		}	
	}

	void process()
	{
		{
			ScopedLock lock(sync);
			if (finalize_ == true){

				finalized_ = true;

				for (auto & c : currentCurls){
					toRemoveCurlsSet.insert(c.first);
				}

				innerRemove();

				for (auto & t : toAddCurlsSet)
				{
					manager->onRemoveRequest(t.first);
					t.second.set_value(CURLE_OK);
				}

				//czuszczê kolejkê do dodania
				CURLsWaitMap().swap(toAddCurlsSet);				

				return;
			}
		}

		//iloæ przetwarzanych transferów
		int runningHandles = 0;
		{
			updateHandles();

			CURLMcode ret = CURLM_OK;

			// tak d³ugo jak curl potrzebuje pozwalam mu operowaæ na danych
			while ((finalize_ == false) && ((ret = curl_multi_perform(multi.get(), &runningHandles)) == CURLM_CALL_MULTI_PERFORM))
			{
				updateHandles();
			};

			// curl zakoñczy³ przetwarzanie dostêpnych danych - mamy chwilê na ich przetworzenie
			if (ret != CURLM_OK && ret != CURLM_CALL_MULTI_PERFORM){
				// wewnêtrzny/krytyczny error przy obs³udze zleceñ - koñczê dalsze przetwarzanie
				//TODO - abort aktualnych uchwytów jeli nie zakoñczy³y poprawnie swoich dzia³añ i lecimy dalej
				//return false;
			}

			// informujemy ¿e ju¿ przetworzylimy dane uchwytów
			manager->onProcess();

			// teraz sprawdzamy status uchwytów
			int messagesInfo = 0;
			CURLMsg * info = nullptr;

			//rezultaty zakoñczonych transferów
			std::vector<std::pair<CURLsWaitMap::iterator, CURLcode>> results;

			while ((info = curl_multi_info_read(multi.get(), &messagesInfo)) != nullptr) {
				// czy uchwyt skoñczy³ ¿¹dane operacje
				if (info->msg == CURLMSG_DONE){
					
					auto it = currentCurls.find(utils::dummyWrap(info->easy_handle));

					results.push_back({ it, info->data.result });

					// jaki jest stan uchwytu
					switch (info->data.result){
					case CURLE_OK:
						manager->onFinishRequest(it->first);
						break;
					case CURLE_ABORTED_BY_CALLBACK:
						manager->onCancelRequest(it->first);
						break;
					default:
						{
							std::string e(curl_easy_strerror(info->data.result));
							manager->onErrorRequest(it->first, e);
						}
						break;					
					}					
				}
			}

			//usuwamy zakoñczone
			for (const auto & res : results){				
				curl_multi_remove_handle(multi.get(), res.first->first.get());
				toRemoveCurlsSet.erase(res.first->first);
				unlockAndRemove(res.first, res.second);			
			}
		}

		if (runningHandles > 0){
			// czekamy a¿ co siê zacznie dziaæ z danymi uchwytów
			int descriptorsNumber = 0;
			while ((finalize_ == false) && (descriptorsNumber == 0)){
				curl_multi_wait(multi.get(), nullptr, 0, waitTime, &descriptorsNumber);
			}
		}
		else{
			//wait condition variable woken up when some new handles are added or removed
			std::unique_lock<std::mutex> lock(waitSync);
			// warunki dodane dla spurious wakeups - jak co jest do zrobienia to robimy
			while (changeToContinue() == false){
				condVar.wait_for(lock, std::chrono::milliseconds(waitTime), [this]{ return changeToContinue(); });
			}
		}		
	}

private:
	//! Czy mamy ju¿ koñczyæ dzia³anie managera
	volatile bool finalize_;
	//! Czy mamy ju¿ koñczyæ dzia³anie managera
	volatile bool finalized_;
	//! Czas oczekiwania na dane [ms]
	const int waitTime = 500;
	//! Obiekt realizuj¹cy czekanie na nowe uchwyty
	std::mutex waitSync;
	//! Conditional variable
	std::condition_variable condVar;
	//! Obiekt do synchronizacji stanu obiektu
	std::mutex sync;
	//! Uchwyt do interfejsu multi realizujacego po³¹czenia
	CURLMPtr multi;
	//! Manager dla którego funkcjonalnoc realizujemy
	CURLManager * manager;
	//! Zbiór uchwytów do dodania
	CURLsWaitMap toAddCurlsSet;
	//! Zbiór uchwytów do usuniêcia
	CURLsSet toRemoveCurlsSet;
	//! Aktualnie obs³ugiwane uchwyty
	CURLsWaitMap currentCurls;
};

CURLManager::CURLManager() : impl(new CURLManagerImpl(createCurlMulti(), this))
{

}

CURLManager::CURLManager(CURLMPtr multi) : impl(new CURLManagerImpl(multi, this))
{
	
}

CURLManager::~CURLManager()
{
}

std::future<CURLcode> CURLManager::addRequest(CURLPtr curl)
{
	return impl->addRequest(curl);
}

void CURLManager::removeRequest(CURLPtr curl)
{
	impl->removeRequest(curl);
}

void CURLManager::run()
{
	impl->run();
}

void CURLManager::finalize()
{
	impl->finalize();
}
