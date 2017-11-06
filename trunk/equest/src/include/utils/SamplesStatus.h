/********************************************************************
	created:  2015/01/12	13:36:58
	filename: SamplesStatus.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__SAMPLESSTATUS_H__
#define __HEADER_GUARD_UTILS__SAMPLESSTATUS_H__

#include <map>
#include <boost/circular_buffer.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/rolling_mean.hpp>

namespace utils
{
	class SamplesStatus
	{
	public:
		//! Mapa statusów - ich granicznych (górnych) wartoœci do faktycznych statusów
		//! Wartoœci powinny byæ w przedziale od 0 do 1, w przeciwnym wypakdu zostsan¹ przeskalowane
		using StatusMap = std::map<float, int>;

		enum SampleType{
			Positive = 2,
			Negative = 4,
			Unspecified = Positive | Negative
		};

	public:
		//! \param windowSize Historia próbek
		//! \param statusMap Mapa statusów
		SamplesStatus(const unsigned int windowSize,
			const StatusMap & statusMap);

		//! \param windowSize Historia próbek
		//! \param statusMap Mapa statusów
		SamplesStatus(const unsigned int windowSize,
			StatusMap && statusMap);

		//! Destruktor
		~SamplesStatus();

		static SamplesStatus createBinaryStatus(const int statusLower, const int statusHigher,
			const float splitValue, const unsigned int windowSize);

		static SamplesStatus createBinaryStatus(const int statusLower, const int statusHigher,
			const int minimumPositiveSamples, const unsigned int windowSize);

		//! Dodajemy pozytywn¹ próbkê
		void positiveSample();
		//! Dodajemy negatywn¹ próbkê
		void negativeSample();
		//! Dodajemy pozytywn¹/negatywn¹ próbkê
		void unspecifiedSample();
		//! Dodajemy próbkê
		//! \param val Wartoœæ próbki
		void sample(const SampleType val);
		//! \return Aktualny status
		int status() const;

	private:
		//! Akumulator dla danych
		mutable boost::accumulators::accumulator_set<float, boost::accumulators::stats<boost::accumulators::tag::rolling_mean>> acc;
		//! Mapa statusów
		const StatusMap statusMap;
	};
}

#endif	// __HEADER_GUARD_UTILS__SAMPLESSTATUS_H__