/********************************************************************
	created:  2011/03/17
	created:  17:3:2011   18:15
	filename: NamedTree.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__DATASTRUCTURE_H__
#define HEADER_GUARD_TIMELINE__DATASTRUCTURE_H__

#include <utils/PtrPolicyHelper.h>
#include <utils/PtrPolicyStd.h>
#include <utils/Debug.h>
#include <utils/SmartPtr.h>
#include <boost/tokenizer.hpp>
#include <type_traits>
#include <vector>

//! forward declaration
class NamedTreeBase;

typedef utils::shared_ptr<NamedTreeBase> NamedTreeBasePtr;
typedef utils::weak_ptr<NamedTreeBase> NamedTreeBaseWPtr;

typedef utils::shared_ptr<const NamedTreeBase> NamedTreeBaseConstPtr;
typedef utils::weak_ptr<const NamedTreeBase> NamedTreeBaseConstWPtr;

class NamedTreeBase : public utils::enable_shared_from_this<NamedTreeBase>
{
public:
	typedef std::vector<NamedTreeBaseConstPtr> ConstChildren;
	typedef ConstChildren::const_iterator const_iterator;
	typedef ConstChildren::size_type size_type;

private:
	//! mapa dzieci które mozna modyfikowac
	typedef std::vector<NamedTreeBasePtr> Children;
	typedef Children::iterator iterator;

	//! definicja tokenizera opartego o boost
	typedef boost::char_separator<char> Separator;
	typedef boost::tokenizer<Separator> Tokenizer;

private:

	static const Separator separator;

	//! Nazwa węzła
	std::string name;

	//! Absolutna sciezka w hierarchi
	std::string absolutePath;

	//! \return Rodzic węzła
	NamedTreeBaseWPtr parent;

	//! \return Rodzic węzła
	NamedTreeBaseConstWPtr constParent;

	//! Dzieci
	Children children;

	//! Dzieci z modyfikatorem const
	ConstChildren constChildren;

public:

	//! Domyślny konstruktor
	//! \param name Nazwa węzła
	NamedTreeBase(const std::string & name = "UnnamedNode");

	//! Konstruktor kopiujący
	//! \param child Wezel zrodlowy do skopiowania
	//! \param deep Glebokość kopiowania - true = glebokie, false płytkie
	NamedTreeBase(const NamedTreeBase & child, bool deep = true);

	virtual ~NamedTreeBase(void);

	//! \return Nazwa węzła
	const std::string & getName() const;

	//! root ma sciezke absolutna ./
	//! \return Absolutna sciezka poczynając od roota
	const std::string & getAbsolutePath() const;

	//! \param name Nazwa węzła
	void setName(std::string name);

	//! \return Pierwsze dziecko
	const_iterator begin() const;

	//! \return Koniec dzieci
	const_iterator end() const;

	//! return Rodzic aktualnego węzła
	NamedTreeBaseConstPtr getParent() const;

	//! return Rodzic aktualnego węzła
	NamedTreeBasePtr getParent();

	//! \param idx Indeks dziecka
	//! \return Dziecko o zadanym indeksie
	const NamedTreeBasePtr & getChild(size_type idx);

	//! \param name Sciezka dziecka
	//! \return Dziecko o zadanej sciezce względem aktualnego węzła
	const NamedTreeBasePtr & getChild(const std::string & path);

	//! \param idx Indeks dziecka
	//! \return Dziecko o zadanym indeksie
	const NamedTreeBaseConstPtr & getChild(size_type idx) const;

	//! \param name Sciezka dziecka
	//! \return Dziecko o zadanej sciezce względem aktualnego węzła
	const NamedTreeBaseConstPtr & getChild(const std::string & path) const;

	//! return Ilość dzieci
	size_type size() const;

	//! \param child Dziecko do dodania
	void addChild(const NamedTreeBasePtr & child);

	//! \param child Dziecka do dodania
	//! \param idx Pozycja na której chcemy dodać dziecko
	void addChild(const NamedTreeBasePtr & child, size_type idx);

	//! \param path Sciezka do wstawienia dziecka
	virtual void addChild(const std::string & path);

	//! \param path Sciezka do wstawienia dziecka
	//! \param idx Pozycja dziecka do wstawienia
	virtual void addChild(const std::string & path, size_type idx);

	//! \param path Sciezka do wstawienia dziecka
	template<class Derrived>
	void addChild(const std::string & path)
	{
		addChild<Derrived>(path, -1);
	}

	template<class Derrived>
	void addChild(const std::string & path, size_type idx) {
		static_assert((std::is_base_of<NamedTreeBase, Derrived>::value), "Base class should inherit from NamedTreeBase");

		UTILS_ASSERT((path.empty() == false), "Bledna sciezka do dziecka!");

		Tokenizer tok(path, separator);

		auto it = tok.begin();

		NamedTreeBasePtr pos = findLastChildInPath(it, tok.end());

		if (it == tok.end()) {
			throw std::runtime_error("Child with the given name already exist!");
		}

		////wyznaczam ostatni poziom, bedacy nazwa dla mojego nowego węzła
		auto itEnd = tok.begin();
		auto itTmp = tok.begin();
		while (++itTmp != tok.end()){
			++itEnd;
		}

		//stworz drzewo do itEnd, a pod itEnd mamy juz nazwę naszego wezsla
		while (it != itEnd){
			NamedTreeBasePtr nodeChild(new Derrived(*it));
			pos->addChild(nodeChild);
			pos = nodeChild;
			++it;
		}

		pos->addChild(NamedTreeBasePtr(new Derrived(*itEnd)), idx);
	}

	//! \param child Dziecko do usuniecia
	void removeChild(const NamedTreeBasePtr & child);

	//! \param idx Pozycja dziecka do usuniecia
	void removeChild(size_type idx);

	//! \param path Sciezka do usuwanego dziecka
	void removeChild(const std::string & path);

	//! \return Root aktualnej struktury
	NamedTreeBasePtr getRoot();

	//! \return Root aktualnej struktury
	NamedTreeBaseConstPtr getRoot() const;

	//! \return Czy element jest lisciem
	bool isLeaf() const;

	//! \return Czy element jest rootem
	bool isRoot() const;

	//! \param name Nazwa dziecka do znalezienia na poziomie dzieci
	//! \return Iterator znalezionego dziecka lub end()
	const_iterator findChildByName(const std::string & name) const;

	//! \param path Sciezka dziecka do znalezienia
	//! \return znalezione dziecka lub nullptr - NamedTreeBasePtr()
	NamedTreeBasePtr findChildByPath(const std::string & path) const;

protected:

	//! Konstruktor na potrzeby tworzenia hierarchi, niedostepny dla użytkownika
	//! \param parent Rodzic węzła - na wewnętrzne potrzeby hierarchi
	//! \param name Nazwa węzła
	NamedTreeBase(const NamedTreeBasePtr & parent, const std::string & name);

	//! \return Pierwsze dziecko
	iterator begin();

	//! \return Koniec dzieci
	iterator end();

	//! \param name Nazwa dziecka do znalezienia
	//! \return Iterator znalezionego dziecka lub end()
	iterator findChildByName(const std::string & name);

	//! \param path Sciezka dziecka do znalezienia
	//! \return znalezione dziecka lub nullptr - NamedTreeBasePtr()
	template<class Iter>
	NamedTreeBasePtr findLastChildInPath(Iter & begin, Iter end) const
	{
		UTILS_ASSERT((begin != end), "Bledna sciezka do dziecka!");

		NamedTreeBaseConstPtr ret = shared_from_this();

		while (begin != end) {
			if ((*begin).empty() == false){
				if (*begin == ".."){
					//jeśli rodzic to znaczy ze chcemy wyjsc poza hierarchie!
					if (ret->isRoot() == true){
						throw std::runtime_error("Want to go higher than root in hierarchy!");
					}

					//do rodzica
					ret = ret->getParent();
					//aktualny poziom - ignoruj
				}
				else if (*begin != ".") {
					//szukaj dziecka na danym poziomie
					auto it = ret->findChildByName(*begin);
					if (it == ret->end()) {
						break;
					}

					ret = *it;
				}
			}
			++begin;
		}

		return utils::const_pointer_cast<NamedTreeBase>(ret);
	}

	//! \param parent Nowy rodzic aktualnej struktury danych
	void setParent(const NamedTreeBasePtr & parent);

	//! resetuje rodzica dziecka na potrzeby usuwania
	void resetParent();

	template<class Iter>
	static Iter findChildByName(Iter begin, Iter end, const std::string & name)
	{
		return std::find_if(begin, end, [&](NamedTreeBaseConstPtr child){ return child->getName() == name; });
	}

	//! Aktualizuje absolutna sciezke węzła
	void updateAbsolutePath();
};

template<class Data, class PtrPolicy>
class NamedTree : public NamedTreeBase, protected PtrPolicy
{
	static_assert((utils::is_ptr_policy<PtrPolicy>::value), "Pointer policy is invalid. This will cause more errors.");

public:
	//!
	//typedef typename PtrPolicy PtrPolicy;
	//!
	typedef PtrPolicy Base;
	//! wskaźnik do danych
	typedef typename Base::template Ptr<Data>::Type Ptr;
	typedef typename Base::template Ptr<const Data>::Type ConstPtr;
	typedef NamedTree<Data, PtrPolicy> NamedTreeType;

private:
	//! Dane
	Ptr data;

	//! Dane z modyfikatorem const
	ConstPtr constData;

public:

	//! Domyślny konstruktor
	//! \param name Nazwa węzła
	//! \param data Dane do przechowania w węzłe, domyślnie brak danych
	NamedTree(const std::string & name = "UnnamedNode", const Ptr & data = Ptr())
		: NamedTreeBase(name)
	{
		if (data != nullptr){
			setData(data);
		}
		else{
			PtrPolicy::initPtr(this->data);
			PtrPolicy::initPtr(this->constData);
		}
	}

	//! Konstruktor kopiujący
	//! \param name Nazwa węzła
	//! \param deep Czy glebokie kopiowanie
	NamedTree(const NamedTree & child, bool deep = true)
		: NamedTreeBase(child, deep)
	{
		if (deep == true){
			setData(new Data(*(child.data)));
		}
		else{
			setData(child.data);
		}
	}

	~NamedTree()
	{
	}

	//! \return dane
	const Ptr & getData()
	{
		return data;
	}

	//! return data
	const ConstPtr & getData() const
	{
		return constData;
	}

	//! \param data Dane do przechowania w węźle
	void setData(const Ptr & data)
	{
		PtrPolicy::template setPtr(this->data, data);
		PtrPolicy::template setPtr(this->constData, data);
	}

	virtual void addChild(const std::string & path)
	{
		NamedTreeBase::addChild<NamedTreeType>(path, -1);
	}

	void addChild(const std::string & path, NamedTreeBase::size_type idx)
	{
		NamedTreeBase::addChild<NamedTreeType>(path, idx);
	}
};

#endif  // HEADER_GUARD_TIMELINE__DATASTRUCTURE_H__
