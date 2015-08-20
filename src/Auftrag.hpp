#ifndef AUFTRAG_HPP_
#define AUFTRAG_HPP_

#include <QObject>
#include <qvariant.h>
#include <QDeclarativeListProperty>
#include <QStringList>
#include <QDate>


#include "Position.hpp"
#include "Schlagwort.hpp"
#include "Kunde.hpp"


class Auftrag: public QObject
{
	Q_OBJECT

	Q_PROPERTY(int nr READ nr WRITE setNr NOTIFY nrChanged FINAL)
	Q_PROPERTY(QDate datum READ datum WRITE setDatum NOTIFY datumChanged FINAL)
	Q_PROPERTY(QString bemerkung READ bemerkung WRITE setBemerkung NOTIFY bemerkungChanged FINAL)
	// auftraggeber lazy pointing to Kunde* (domainKey: nr)
	Q_PROPERTY(int auftraggeber READ auftraggeber WRITE setAuftraggeber NOTIFY auftraggeberChanged FINAL)
	Q_PROPERTY(Kunde* auftraggeberAsDataObject READ auftraggeberAsDataObject WRITE resolveAuftraggeberAsDataObject NOTIFY auftraggeberAsDataObjectChanged FINAL)

	// QDeclarativeListProperty to get easy access from QML
	Q_PROPERTY(QDeclarativeListProperty<Position> positionenPropertyList READ positionenPropertyList CONSTANT)
	// QDeclarativeListProperty to get easy access from QML
	Q_PROPERTY(QDeclarativeListProperty<Schlagwort> tagsPropertyList READ tagsPropertyList CONSTANT)

public:
	Auftrag(QObject *parent = 0);

	Q_INVOKABLE
	bool isAllResolved();

	void fillFromMap(const QVariantMap& auftragMap);
	void fillFromForeignMap(const QVariantMap& auftragMap);
	void fillFromCacheMap(const QVariantMap& auftragMap);
	
	void prepareNew();
	
	bool isValid();

	Q_INVOKABLE
	QVariantMap toMap();
	QVariantMap toForeignMap();
	QVariantMap toCacheMap();

	int nr() const;
	void setNr(int nr);
	QDate datum() const;

	Q_INVOKABLE
	bool hasDatum();
	void setDatum(QDate datum);
	QString bemerkung() const;
	void setBemerkung(QString bemerkung);
	// auftraggeber lazy pointing to Kunde* (domainKey: nr)
	int auftraggeber() const;
	void setAuftraggeber(int auftraggeber);
	Kunde* auftraggeberAsDataObject() const;
	
	Q_INVOKABLE
	void resolveAuftraggeberAsDataObject(Kunde* kunde);
	
	Q_INVOKABLE
	void removeAuftraggeber();
	
	Q_INVOKABLE
	bool hasAuftraggeber();
	
	Q_INVOKABLE
	bool isAuftraggeberResolvedAsDataObject();
	
	Q_INVOKABLE
	void markAuftraggeberAsInvalid();
	

	
	Q_INVOKABLE
	QVariantList positionenAsQVariantList();

	Q_INVOKABLE
	Position* createElementOfPositionen();

	Q_INVOKABLE
	void undoCreateElementOfPositionen(Position* position);
	
	Q_INVOKABLE
	void addToPositionen(Position* position);
	
	Q_INVOKABLE
	bool removeFromPositionen(Position* position);

	Q_INVOKABLE
	void clearPositionen();

	Q_INVOKABLE
	void addToPositionenFromMap(const QVariantMap& positionMap);
	
	Q_INVOKABLE
	bool removeFromPositionenByUuid(const QString& uuid);
	
	Q_INVOKABLE
	int positionenCount();
	
	 // access from C++ to positionen
	QList<Position*> positionen();
	void setPositionen(QList<Position*> positionen);
	// access from QML to positionen
	QDeclarativeListProperty<Position> positionenPropertyList();
	
	Q_INVOKABLE
	QVariantList tagsAsQVariantList();

	
	Q_INVOKABLE
	void addToTags(Schlagwort* schlagwort);
	
	Q_INVOKABLE
	bool removeFromTags(Schlagwort* schlagwort);

	Q_INVOKABLE
	void clearTags();

	// lazy Array of independent Data Objects: only keys are persisted
	Q_INVOKABLE
	bool areTagsKeysResolved();

	Q_INVOKABLE
	QStringList tagsKeys();

	Q_INVOKABLE
	void resolveTagsKeys(QList<Schlagwort*> tags);
	
	Q_INVOKABLE
	int tagsCount();
	
	 // access from C++ to tags
	QList<Schlagwort*> tags();
	void setTags(QList<Schlagwort*> tags);
	// access from QML to tags
	QDeclarativeListProperty<Schlagwort> tagsPropertyList();


	virtual ~Auftrag();

	Q_SIGNALS:

	void nrChanged(int nr);
	void datumChanged(QDate datum);
	void bemerkungChanged(QString bemerkung);
	// auftraggeber lazy pointing to Kunde* (domainKey: nr)
	void auftraggeberChanged(int auftraggeber);
	void auftraggeberAsDataObjectChanged(Kunde* kunde);
	void positionenChanged(QList<Position*> positionen);
	void addedToPositionen(Position* position);
	void removedFromPositionenByUuid(QString uuid);
	
	void tagsChanged(QList<Schlagwort*> tags);
	void addedToTags(Schlagwort* schlagwort);
	
	

private:

	int mNr;
	QDate mDatum;
	QString mBemerkung;
	int mAuftraggeber;
	bool mAuftraggeberInvalid;
	Kunde* mAuftraggeberAsDataObject;
	QList<Position*> mPositionen;
	// implementation for QDeclarativeListProperty to use
	// QML functions for List of Position*
	static void appendToPositionenProperty(QDeclarativeListProperty<Position> *positionenList,
		Position* position);
	static int positionenPropertyCount(QDeclarativeListProperty<Position> *positionenList);
	static Position* atPositionenProperty(QDeclarativeListProperty<Position> *positionenList, int pos);
	static void clearPositionenProperty(QDeclarativeListProperty<Position> *positionenList);
	// lazy Array of independent Data Objects: only keys are persisted
	QStringList mTagsKeys;
	bool mTagsKeysResolved;
	QList<Schlagwort*> mTags;
	// implementation for QDeclarativeListProperty to use
	// QML functions for List of Schlagwort*
	static void appendToTagsProperty(QDeclarativeListProperty<Schlagwort> *tagsList,
		Schlagwort* schlagwort);
	static int tagsPropertyCount(QDeclarativeListProperty<Schlagwort> *tagsList);
	static Schlagwort* atTagsProperty(QDeclarativeListProperty<Schlagwort> *tagsList, int pos);
	static void clearTagsProperty(QDeclarativeListProperty<Schlagwort> *tagsList);

	Q_DISABLE_COPY (Auftrag)
};
Q_DECLARE_METATYPE(Auftrag*)

#endif /* AUFTRAG_HPP_ */

