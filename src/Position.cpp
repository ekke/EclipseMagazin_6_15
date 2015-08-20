#include "Position.hpp"
#include <QDebug>
#include <quuid.h>
#include "Auftrag.hpp"

// keys of QVariantMap used in this APP
static const QString uuidKey = "uuid";
static const QString bezeichnungKey = "bezeichnung";
static const QString PreisKey = "Preis";
// no key for auftragsKopf

// keys used from Server API etc
static const QString uuidForeignKey = "uuid";
static const QString bezeichnungForeignKey = "bezeichnung";
static const QString PreisForeignKey = "Preis";
// no key for auftragsKopf

/*
 * Default Constructor if Position not initialized from QVariantMap
 */
Position::Position(QObject *parent) :
        QObject(parent), mUuid(""), mBezeichnung(""), mPreis(0.0)
{
}

/*
 * initialize Position from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses own property names
 * corresponding export method: toMap()
 */
void Position::fillFromMap(const QVariantMap& positionMap)
{
	mUuid = positionMap.value(uuidKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mBezeichnung = positionMap.value(bezeichnungKey).toString();
	mPreis = positionMap.value(PreisKey).toDouble();
	// mAuftragsKopf is parent (Auftrag* containing Position)
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses foreign property names - per ex. from Server API
 * corresponding export method: toForeignMap()
 */
void Position::fillFromForeignMap(const QVariantMap& positionMap)
{
	mUuid = positionMap.value(uuidForeignKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mBezeichnung = positionMap.value(bezeichnungForeignKey).toString();
	mPreis = positionMap.value(PreisForeignKey).toDouble();
	// mAuftragsKopf is parent (Auftrag* containing Position)
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * excludes transient values
 * uses own property names
 * corresponding export method: toCacheMap()
 */
void Position::fillFromCacheMap(const QVariantMap& positionMap)
{
	mUuid = positionMap.value(uuidKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mBezeichnung = positionMap.value(bezeichnungKey).toString();
	mPreis = positionMap.value(PreisKey).toDouble();
	// mAuftragsKopf is parent (Auftrag* containing Position)
}

void Position::prepareNew()
{
	mUuid = QUuid::createUuid().toString();
	mUuid = mUuid.right(mUuid.length() - 1);
	mUuid = mUuid.left(mUuid.length() - 1);
}

/*
 * Checks if all mandatory attributes, all DomainKeys and uuid's are filled
 */
bool Position::isValid()
{
	if (mUuid.isNull() || mUuid.isEmpty()) {
		return false;
	}
	return true;
}
	
/*
 * Exports Properties from Position as QVariantMap
 * exports ALL data including transient properties
 * To store persistent Data in JsonDataAccess use toCacheMap()
 */
QVariantMap Position::toMap()
{
	QVariantMap positionMap;
	positionMap.insert(uuidKey, mUuid);
	positionMap.insert(bezeichnungKey, mBezeichnung);
	positionMap.insert(PreisKey, mPreis);
	// mAuftragsKopf points to Auftrag* containing Position
	return positionMap;
}

/*
 * Exports Properties from Position as QVariantMap
 * To send data as payload to Server
 * Makes it possible to use defferent naming conditions
 */
QVariantMap Position::toForeignMap()
{
	QVariantMap positionMap;
	positionMap.insert(uuidForeignKey, mUuid);
	positionMap.insert(bezeichnungForeignKey, mBezeichnung);
	positionMap.insert(PreisForeignKey, mPreis);
	// mAuftragsKopf points to Auftrag* containing Position
	return positionMap;
}


/*
 * Exports Properties from Position as QVariantMap
 * transient properties are excluded:
 * To export ALL data use toMap()
 */
QVariantMap Position::toCacheMap()
{
	// no transient properties found from data model
	// use default toMao()
	return toMap();
}
// ATT 
// Mandatory: uuid
// Domain KEY: uuid
QString Position::uuid() const
{
	return mUuid;
}

void Position::setUuid(QString uuid)
{
	if (uuid != mUuid) {
		mUuid = uuid;
		emit uuidChanged(uuid);
	}
}
// ATT 
// Optional: bezeichnung
QString Position::bezeichnung() const
{
	return mBezeichnung;
}

void Position::setBezeichnung(QString bezeichnung)
{
	if (bezeichnung != mBezeichnung) {
		mBezeichnung = bezeichnung;
		emit bezeichnungChanged(bezeichnung);
	}
}
// ATT 
// Optional: Preis
double Position::Preis() const
{
	return mPreis;
}

void Position::setPreis(double Preis)
{
	if (Preis != mPreis) {
		mPreis = Preis;
		emit PreisChanged(Preis);
	}
}
// REF
// Opposite: positionen
// Optional: auftragsKopf
// No SETTER for AuftragsKopf - it's the parent
Auftrag* Position::auftragsKopf() const
{
	return qobject_cast<Auftrag*>(parent());
}


Position::~Position()
{
	// place cleanUp code here
}
	
