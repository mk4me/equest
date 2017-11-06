/********************************************************************
	created:  2014/10/03	10:55:44
	filename: Queue.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__QUEUE_H__
#define __HEADER_GUARD_THREADINGUTILS__QUEUE_H__

#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>
#include <chrono>
#include <threadingUtils/StealingQueue.h>

namespace threadingUtils
{
	//! Kolejka synchronizowana
	//! \tparam T Typ danych przechowywanych w kolejce
	template<typename T>
	class Queue
	{
	private:
		//! Obiket synchornizujacy
		mutable std::mutex mutex;
		//! Kolejka danych
		std::queue<T> queue;
		//! Zmienna warunkowa dla synchornizacji
		std::condition_variable condition;

	public:

		//! Typ opisuący rozmiar naszej kolejki
		using size_type = typename std::queue<T>::size_type;

	public:
		//! Konsturktor domyslny
		Queue()
		{

		}

		//! Konsturktor kopiujący
		//! \param other Kopiowany obiekt
		Queue(const Queue & other)
		{
			std::lock_guard<std::mutex> lk(other.mutex);
			queue = other.queue;
		}
		//! Destruktor
		~Queue()
		{

		};

		//! \param value Dane wrzucane do kolejki
		void push(T value)
		{
			std::lock_guard<std::mutex> lk(mutex);
			queue.push(std::move(value));
			condition.notify_one();
		}		

		//! \param value [out] Wartość do której zapiszemy ściągany element kolejki
		void waitAndPop(T& value)
		{
			std::unique_lock<std::mutex> lk(mutex);
			condition.wait(lk, [this]{return !queue.empty(); });
			value = std::move(queue.front());
			queue.pop();
		}

		//! \return Ściągnięty element z kolejki
		std::shared_ptr<T> waitAndPop()
		{
			std::unique_lock<std::mutex> lk(mutex);
			condition.wait(lk, [this]{return !queue.empty(); });
			std::shared_ptr<T> res(utils::make_shared<T>(std::move(queue.front())));
			queue.pop();
			return res;
		}

		//! \tparam Rep
		//! \tparam Period
		//! \param value [out] Wartośc do której będzie zapisany ściągnięty z kolejki element
		//! \param duration Czas czekania na dane
		//! \return Czy udało się ściągnąc dane z kolejki
		template<class Rep = long long,
		class Period = std::chrono::milliseconds>
		bool tryWaitAndPop(T& value, const std::chrono::duration<Rep, Period> & duration)
		{
			std::unique_lock<std::mutex> lk(mutex);
			condition.wait_for(lk, duration);
			if (queue.empty() == true){
				return false;
			}

			value = std::move(queue.front());
			queue.pop();
			return true;
		}

		//! \tparam Rep
		//! \tparam Period		
		//! \param duration Czas czekania na dane
		//! \return Ściągnięta wartośc z kolejki lub nullptr jeśli się nie udało
		template<class Rep = long long,
		class Period = std::chrono::milliseconds>
		std::shared_ptr<T> tryWaitAndPop(const std::chrono::duration<Rep, Period> & duration)
		{
			std::unique_lock<std::mutex> lk(mutex);
			condition.wait_for(lk, duration);
			if (queue.empty() == true){
				return std::shared_ptr<T>();
			}

			std::shared_ptr<T> res(utils::make_shared<T>(std::move(queue.front())));
			queue.pop();
			return res;
		}

		//! \param value [out] Wartośc do której będzie zapisany ściągnięty z kolejki element
		//! \return Czy udało się ściągnać wartość
		bool tryPop(T& value)
		{
			std::lock_guard<std::mutex> lk(mutex);
			if (queue.empty() == true)
				return false;
			value = std::move(queue.front());
			queue.pop();
			return true;
		}

		//! \return Ściągnięta wartośc z kolejki lub nullptr jeśli się nie udało
		std::shared_ptr<T> tryPop()
		{
			std::lock_guard<std::mutex> lk(mutex);
			if (queue.empty() == true)
				return std::shared_ptr<T>();
			std::shared_ptr<T> res(utils::make_shared<T>(std::move(queue.front())));
			queue.pop();
			return res;
		}

		//! \return Czy kolejka jest pusta
		bool empty() const
		{
			std::lock_guard<std::mutex> lk(mutex);
			return queue.empty();
		}

		size_type size() const
		{			
			std::lock_guard<std::mutex> lk(mutex);
			return queue.size();
		}
		
		//! Metoda czyści zawartośc kolejki
		void clear()
		{
			std::lock_guard<std::mutex> lk(mutex);
			std::queue<T>().swap(queue);
		}
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__QUEUE_H__
