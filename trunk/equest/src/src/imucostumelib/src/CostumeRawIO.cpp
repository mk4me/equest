#include <imucostumelib/CostumeRawIO.h>
#include <boost/asio.hpp>
#include <utils/Clamping.h>

using namespace imuCostume;

static const uint8_t MagicNumberLength = 16;
static const uint8_t PingPongPacketLength = MagicNumberLength + 2;

static const uint8_t PingMagicNumber[MagicNumberLength] = { 0xB4, 0xAE, 0x7B, 0xB8, 0x71, 0xF4, 0x4E, 0x9A, 0xBC, 0xBD, 0x60, 0xEF, 0x68, 0x0C, 0xAE, 0xE9 };
static const uint8_t PongMagicNumber[MagicNumberLength] = { 0x7E, 0x5F, 0xC1, 0xB6, 0xB6, 0x8F, 0x4A, 0xD3, 0xBD, 0x37, 0x91, 0x9B, 0x89, 0xD9, 0x98, 0x4A };
static const uint32_t Port = 1234;

class CostumeRawIO::CostumeRawIOImpl
{
public:
	CostumeRawIOImpl(const std::string & ip, const unsigned int port) :
		ip_(ip), port_(port), io_service(), socket(io_service), samplingDelay_(0)
	{
		socket.open(boost::asio::ip::udp::v4());
		socket.set_option(boost::asio::socket_base::reuse_address(true));
		socket.bind(boost::asio::ip::udp::endpoint(	boost::asio::ip::address_v4::any(),	port));
		socket.connect(boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(ip), port));
		setSamplingDelay(20, 100);
	}

	~CostumeRawIOImpl()
	{
	}

	const std::string & ip() const
	{
		return ip_;
	}

	unsigned int port() const
	{
		return port_;
	}

	bool receive(CostumeRawIO::Buffer & buffer, uint16_t & length, const uint16_t timeout)
	{
		std::size_t l = 0;
		CostumeRawIO::Buffer locBuffer = { 0 };
		bool transferOK = true;

		if (timeout == 0){
			try{
				l = socket.receive(boost::asio::buffer(locBuffer));
			}
			catch (...){
				transferOK = false;
			}
		}
		else{
			boost::system::error_code ec = boost::asio::error::would_block;
			//! Ograniczenie czasowe na odbiór danych
			boost::asio::deadline_timer deadline(io_service);
			deadline.expires_from_now(boost::posix_time::millisec(timeout));

			// Put the actor back to sleep.
			deadline.async_wait(std::bind(&CostumeRawIOImpl::check_deadline, &deadline, &socket));

			socket.async_receive(boost::asio::buffer(locBuffer),
				std::bind(&CostumeRawIOImpl::handle_operation, std::placeholders::_1, std::placeholders::_2, &ec, &l));			

			// Block until the asynchronous operation has completed.
			try{
				do {
					io_service.run_one();					
				} while (ec == boost::asio::error::would_block);

				if (ec == boost::asio::error::operation_aborted){
					transferOK = false;
				}
			}
			catch (...){
				transferOK = false;
			}

			deadline.cancel();
		}

		transferOK = ((transferOK == true) && (l > 0));

		if (transferOK == true){
			length = l;
			buffer = locBuffer;
		}

		return transferOK;
	}

	bool send(const void * data, const uint16_t length, const uint16_t timeout)
	{
		std::size_t l = 0;

		if (timeout == 0){
			try{
				l = socket.send(boost::asio::buffer(data, length));
			}
			catch (...){
			}
		}
		else{
			boost::system::error_code ec = boost::asio::error::would_block;
			//! Ograniczenie czasowe na odbiór danych
			boost::asio::deadline_timer deadline(io_service);
			deadline.expires_from_now(boost::posix_time::millisec(timeout));

			// Put the actor back to sleep.
			deadline.async_wait(std::bind(&CostumeRawIOImpl::check_deadline, &deadline, &socket));

			socket.async_send(boost::asio::buffer(data, length),
				std::bind(&CostumeRawIOImpl::handle_operation, std::placeholders::_1, std::placeholders::_2, &ec, &l));

			// Block until the asynchronous operation has completed.
			try{
				do {
					io_service.run_one();
				} while (ec == boost::asio::error::would_block);
			}
			catch (...){
			}

			deadline.cancel();
		}

		return (l == length);
	}

	//OK
	bool setSamplingDelay(const uint32_t delay, const uint16_t timeout)
	{
		auto d = utils::clamp<uint8_t>(delay / 10, 1, 255);
		std::array<uint8_t, 19> buffer;
		buffer[0] = 0x04;
		buffer[1] = (uint8_t)this;
		std::memcpy(buffer.data() + 2, PingMagicNumber, MagicNumberLength);
		buffer[18] = d;
		auto ret = send(buffer.data(), 19, timeout);
		if (ret == true){
			samplingDelay_ = delay;
		}

		return ret;
	}

	uint32_t samplingDelay() const
	{
		return samplingDelay_;
	}

	//OK
	static std::list<CostumeRawIO::CostumeAddress> listAvailableCostumes()
	{
		std::list<CostumeRawIO::CostumeAddress> ret;

		static const boost::asio::ip::udp::endpoint broadcastEndpoint = boost::asio::ip::udp::endpoint(
			boost::asio::ip::address_v4::broadcast(), Port);


		//! Serwis
		boost::asio::io_service io_service;
		//! Socket
		boost::asio::ip::udp::socket socket(io_service);
		socket.open(broadcastEndpoint.protocol());
		socket.set_option(boost::asio::socket_base::broadcast(true));
		socket.set_option(boost::asio::socket_base::reuse_address(true));
		socket.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::any(), 1234));

		std::array<uint8_t, PingPongPacketLength> pingMessage = { 0 };
		pingMessage[0] = pingMessage[1] = 0x03;
		std::memcpy(pingMessage.data() + 2, PingMagicNumber, MagicNumberLength);

		const auto sl = socket.send_to(boost::asio::buffer(pingMessage),
			boost::asio::ip::udp::endpoint(boost::asio::ip::udp::endpoint(
			boost::asio::ip::address_v4::broadcast(), Port)));

		socket.set_option(boost::asio::socket_base::broadcast(false));

		if (sl == PingPongPacketLength){

			uint16_t i = 0;
			uint8_t sCounter = 0;

			bool silence = false;

			while ((silence == false) && (i++ < 255 * 5))
			{
				boost::asio::ip::udp::endpoint senderEndpoint;

				std::size_t l = 0;
				CostumeRawIO::Buffer locFrame = { 0 };

				boost::system::error_code ec = boost::asio::error::would_block;
				//! Ograniczenie czasowe na odbiór danych
				boost::asio::deadline_timer deadline(io_service);
				deadline.expires_from_now(boost::posix_time::millisec(100));

				// Put the actor back to sleep.
				deadline.async_wait(std::bind(&CostumeRawIOImpl::check_deadline, &deadline, &socket));

				socket.async_receive_from(boost::asio::buffer(locFrame), senderEndpoint,
					std::bind(&CostumeRawIOImpl::handle_operation, std::placeholders::_1, std::placeholders::_2, &ec, &l));

				bool transferOK = true;

				// Block until the asynchronous operation has completed.
				try{
					do {
						io_service.run_one();
					} while (ec == boost::asio::error::would_block);

					if (ec == boost::asio::error::operation_aborted){
						transferOK = false;
					}
				}
				catch (...){
					transferOK = false;
				}

				if (transferOK == false){
					++sCounter;
					silence = !(sCounter < 10);
				}
				else{
					sCounter = 0;
				}

				deadline.cancel();

				if (l == PingPongPacketLength){

					if ((locFrame[0] == (CostumeRawIO::Buffer::value_type)0x83) &&
						(locFrame[1] == (CostumeRawIO::Buffer::value_type)0x03) &&
						(std::memcmp(PongMagicNumber, locFrame.data() + 2, MagicNumberLength) == 0)){
						ret.push_back(CostumeRawIO::CostumeAddress{ senderEndpoint.address().to_string(), senderEndpoint.port() });
						if (ret.size() == 254){
							break;
						}
					}
				}				
			}
		}

		return ret;
	}

private:

	static void check_deadline(const boost::asio::deadline_timer * deadline,
		boost::asio::ip::udp::socket * socket)
	{
		// Check whether the deadline has passed. We compare the deadline against
		// the current time since a new asynchronous operation may have moved the
		// deadline before this actor had a chance to run.
		if (deadline->expires_at() <= boost::asio::deadline_timer::traits_type::now())
		{
			// The deadline has passed. The outstanding asynchronous operation needs
			// to be cancelled so that the blocked receive() function will return.
			//
			// Please note that cancel() has portability issues on some versions of
			// Microsoft Windows, and it may be necessary to use close() instead.
			// Consult the documentation for cancel() for further information.
			socket->cancel();
		}
	}

	static void handle_operation(
		const boost::system::error_code& ec, std::size_t length,
		boost::system::error_code* out_ec, std::size_t* out_length)
	{
		*out_ec = ec;
		*out_length = length;
	}

private:

	//! Adres IP kostiumu
	const std::string ip_;
	//! Port kostiumy
	const unsigned int port_;
	//!
	uint8_t samplingDelay_;
	//! Obiekty do odbierania danych po UDP
	//! Serwis
	boost::asio::io_service io_service;
	//! Socket
	boost::asio::ip::udp::socket socket;
};

CostumeRawIO::CostumeRawIO(const std::string & ip, const unsigned int port)
	: impl(new CostumeRawIOImpl(ip, port))
{

}

CostumeRawIO::CostumeRawIO(const CostumeRawIO & Other)
	: impl(new CostumeRawIOImpl(Other.ip(), Other.port()))
{

}

CostumeRawIO::~CostumeRawIO()
{
}

const std::string & CostumeRawIO::ip() const
{
	return impl->ip();
}

unsigned int CostumeRawIO::port() const
{
	return impl->port();
}

bool CostumeRawIO::receive(Buffer & buffer, uint16_t & length, const uint16_t timeout)
{
	return impl->receive(buffer, length, timeout);
}

bool CostumeRawIO::send(const void * data, const uint16_t length, const uint16_t timeout)
{
	return impl->send(data, length, timeout);
}

void CostumeRawIO::setSamplingDelay(const uint32_t delay, const uint16_t timeout)
{
	impl->setSamplingDelay(delay, timeout);
}

std::list<CostumeRawIO::CostumeAddress> CostumeRawIO::listAvailableCostumes()
{
	return CostumeRawIOImpl::listAvailableCostumes();
}

uint32_t CostumeRawIO::samplingDelay() const
{
	return impl->samplingDelay();
}