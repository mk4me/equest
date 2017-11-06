/********************************************************************
	created:  2014/10/06	19:10:48
	filename: StealingQueue.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__STEALINGQUEUE_H__
#define __HEADER_GUARD_THREADINGUTILS__STEALINGQUEUE_H__

#include <deque>
#include <mutex>

namespace threadingUtils
{
	//! Klasa realizuj¹ca kolejkê danych umo¿liwiaj¹c œci¹gniêcie danych z jej koñca (kradzie¿)
	//! \tparam T Typ danych przechowywanych w kolejce
	template<typename T>
	class StealingQueue
	{
	private:
		//! Kolejka danych
		std::deque<T> queue;
		//! Obiekt synchronizuj¹cy
		mutable std::mutex mutex;

	public:

		//! Typ opisuj¹cy rozmiar kolejki
		using size_type = typename std::deque<T>::size_type;

	public:
		//! Konstruktor domyœlny
		StealingQueue()
		{

		}

		//! Destruktor
		~StealingQueue()
		{

		}

		StealingQueue(const StealingQueue& other) = delete;
		StealingQueue& operator=(const StealingQueue& other) = delete;

		//! \param data Dane wrzucane do kolejki
		void push(T && data)
		{
			std::lock_guard<std::mutex> lock(mutex);
			queue.push_front(std::move(data));
		}		

		//! \param data Dane wrzucane do kolejki
		void push(const T & data)
		{
			std::lock_guard<std::mutex> lock(mutex);
			queue.push_front(data);
		}

		//! \return Czy kolejka jest pusta
		bool empty() const
		{
			std::lock_guard<std::mutex> lock(mutex);
			return queue.empty();
		}

		//! \return Czy kolejka jest pusta
		size_type size() const
		{
			std::lock_guard<std::mutex> lock(mutex);
			return queue.size();
		}

		//! \param data [out] Obiekt w którym zapiszemy wartoœæ œci¹gniêt¹ z kolejki
		//! \return Czy uda³o siê œci¹gn¹æ dane z kolejki
		bool tryPop(T & data)
		{
			std::lock_guard<std::mutex> lock(mutex);
			if (queue.empty())
			{
				return false;
			}

			data = std::move(queue.front());
			queue.pop_front();
			return true;
		}

		//! \param data [out] Obiekt w którym zapiszemy wartoœæ œci¹gniêt¹ z koñca kolejki (kradzie¿)
		//! \return Czy uda³o siê œci¹gn¹æ dane z koñca kolejki
		bool trySteal(T & data)
		{
			std::lock_guard<std::mutex> lock(mutex);
			if (queue.empty())
			{
				return false;
			}

			data = std::move(queue.back());
			queue.pop_back();
			return true;
		}

		//! Metoda czyœci kolejkê
		void clear()
		{
			std::lock_guard<std::mutex> lock(mutex);
			std::deque<T>().swap(queue);
		}
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__STEALINGQUEUE_H__