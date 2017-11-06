#ifndef HEADER_GUARD_KINEMATICMODEL__BVHPARSER_H__
#define HEADER_GUARD_KINEMATICMODEL__BVHPARSER_H__

#include <biovisionformatslib/Skeleton.h>
#include <biovisionformatslib/MotionData.h>

namespace biovision
{

//! Struktura opisująca dane BVH
struct BVHData
{
	//! Szkielet
	Skeleton model;
	//! Dane ruchu
	MotionData data;
};
DEFINE_SMART_POINTERS(BVHData);


/// \brief Klasa realizuje odczyt i zapis plików *.bvh
class BvhParser
{
public:
    BvhParser();
    virtual ~BvhParser();

public:
    /// \brief  Zapisuje dane z modelu do pliku *.asf
    /// \param  model reprezentacja wewnętrzna animacji
    /// \param  filename nazwa pliku, do którego zostaną zapisane dane
    void save(const Skeleton & model, const MotionData & data, const std::string& filename );
    /// \brief  Parsuje podany plik. 
	BVHData parse(std::istream & in);

private:			
	//! jeśli true, to wymuszony zostaje format kanałów (3 RZ,RX,RY lub 6 TX,TY,TZ,RZ,RX,RY)
    bool forceZXY; 	
	//! jeśli ustawione, to w pliku zostaną wstawione tabulatory, spacje w przeciwnym razie
    bool tabs;     

private:
    /// \brief  Wczytuje pojedynczego jointa z pliku. Wywoływane rekurencyjnie
    /// \param [in] in strumień wejściowy. 
    /// \param [in,out] jointList Lista, do której trafia kolejność wystapienia jointow. 
	void readSingleJoint(Skeleton & model, std::istream& in, std::vector<JointPtr>& jointList, JointPtr parent);
    /// \brief  Zwraca nazwę kanału (w formacie biovision, np. xPosition)
    /// \param  channel kanał, dla którego zostanie zwrócona nazwa. 
    /// \return The channel name. 
    static std::string getChannelName(const kinematicUtils::Channel channel);

	static kinematicUtils::Channel getChannel(const std::string & channelName);
    /// \brief  Pobiera wartości kanałów dla konkretnej kości i konkretnej klatki. 
    /// \param  frameNo     Nr klatki animacji 
    /// \param  boneName    Nazwa kości
    /// \return Stan kości, z wartościami kanałów. 
	//std::ve getSingleJointState(MotionData & data, int frameNo, const std::string & jointName) const;
    /// \brief  Zapisuje pojedynczy staw
	/// \param model				Model ze szkieletem
    /// \param [in,out] out         strumień wyjsciowy
    /// \param  bone                Kość do zapisu 
    /// \param  parent				Kość rodzica (przydatne przy sprawdzaniu i usuwaniu dummy bones).
    /// \param  lvl                 Poziom rekurencji (do wciec w pliku) 
    /// \param [in,out] jointList   Do listy trafiaja w odpowiedniej kolejności nazwy zapisywanych kości 
    //void saveJoint(const Skeleton & model, std::ostream& out, JointConstPtr joint, JointConstPtr parent, int lvl, std::list<std::string>& jointList) const;
    /// \brief  Zapisuje kolejność zapisu kanałów dla jointa 
    /// \param [in]        strumień wyjsciowy. 
    /// \param  bone       Kość, dla której będzie zapisana hierarchia. 
    /// \param  lvl        Poziom rekurencji (do wciec w pliku) 
    void saveChannelsInHierarchy(std::ostream& out, JointConstPtr joint, int lvl) const;
    /// \brief  Czy podane stopnie swobody zawieraja tez dotyczace przesuniecia
    /// \param [in] dofs    Wektor ze stopniami swobody 
    /// \return true jeśli znaleziono kanał z transformacja. 
	static bool hasTranslation(const std::vector<kinematicUtils::Channel>& channels);
    /// \brief  Zwraca stringa zawierającego liczbę spacji lub tabulatorów uzależniona od poziomu rekurencji
    std::string spaceL(int lvl) const;

private:
	//! stos, umozliwia popranwe ustalenie zależności rodzic - dziecko (w rekurencji)
    std::vector<std::string> tempParentVector;
};
}

#endif
