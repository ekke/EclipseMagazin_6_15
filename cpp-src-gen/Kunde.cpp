#include "Kunde.hpp"
#include <QDebug>
#include <quuid.h>

// keys of QVariantMap used in this APP
static const QString nrKey = "nr";
static const QString nameKey = "name";
static const QString ortKey = "ort";

// keys used from Server API etc
static const QString nrForeignKey = "nr";
static const QString nameForeignKey = "name";
static const QString ortForeignKey = "ort";
// SQL
static bool queryPosInitialized;
static int nrQueryPos;
static int nameQueryPos;
static int ortQueryPos;

/*
 * Default Constructor if Kunde not initialized from QVariantMap
 */
Kunde::Kunde(QObject *parent) :
        QObject(parent), mNr(-1), mName(""), mOrt("")
{
}
// S Q L
const QString Kunde::createTableCommand()
{
	QString createSQL = "CREATE TABLE kunde (";
	// nr
	createSQL.append(nrKey).append(" INTEGER");
	createSQL.append(" PRIMARY KEY");
	createSQL.append(", ");
	// name
	createSQL.append(nameKey).append(" TEXT");
	createSQL.append(", ");
	// ort
	createSQL.append(ortKey).append(" TEXT");
	createSQL.append(", ");
	//
    createSQL = createSQL.left(createSQL.length()-2);
    createSQL.append(");");
    return createSQL;
}
const QString Kunde::createParameterizedInsertNameBinding()
{
	QString insertSQL;
    QString valueSQL;
    insertSQL = "INSERT INTO kunde (";
    valueSQL = " VALUES (";
// nr 
	insertSQL.append(nrKey);
	insertSQL.append(", ");
	valueSQL.append(":");
	valueSQL.append(nrKey);
	valueSQL.append(", ");
// name 
	insertSQL.append(nameKey);
	insertSQL.append(", ");
	valueSQL.append(":");
	valueSQL.append(nameKey);
	valueSQL.append(", ");
// ort 
	insertSQL.append(ortKey);
	insertSQL.append(", ");
	valueSQL.append(":");
	valueSQL.append(ortKey);
	valueSQL.append(", ");
//
    insertSQL = insertSQL.left(insertSQL.length()-2);
    insertSQL.append(") ");
    valueSQL = valueSQL.left(valueSQL.length()-2);
    valueSQL.append(") ");
    insertSQL.append(valueSQL);
    return insertSQL;
}
const QString Kunde::createParameterizedInsertPosBinding()
{
	QString insertSQL;
    QString valueSQL;
    insertSQL = "INSERT INTO kunde (";
    valueSQL = " VALUES (";
// nr 
	insertSQL.append(nrKey);
	insertSQL.append(", ");
	valueSQL.append("?, ");
// name 
	insertSQL.append(nameKey);
	insertSQL.append(", ");
	valueSQL.append("?, ");
// ort 
	insertSQL.append(ortKey);
	insertSQL.append(", ");
	valueSQL.append("?, ");
//
    insertSQL = insertSQL.left(insertSQL.length()-2);
    insertSQL.append(") ");
    valueSQL = valueSQL.left(valueSQL.length()-2);
    valueSQL.append(") ");
    insertSQL.append(valueSQL);
    return insertSQL;
}
/*
 * Exports Properties from Kunde as QVariantLists
 * to insert into SQLite
 * 
 * To cache as JSON use toCacheMap()
 */
void Kunde::toSqlCache(QVariantList& nrList, QVariantList& nameList, QVariantList& ortList)
{
	nrList << mNr;
	nameList << mName;
	ortList << mOrt;
}
void Kunde::fillSqlQueryPos(const QSqlRecord& record)
{
    if(queryPosInitialized) {
        return;
    }
nrQueryPos = record.indexOf(nrKey);
nameQueryPos = record.indexOf(nameKey);
ortQueryPos = record.indexOf(ortKey);
}
/*
 * initialize Kunde from QSqlQuery
 * corresponding export method: toSqlMap()
 */
void Kunde::fillFromSqlQuery(const QSqlQuery& sqlQuery)
{
	mNr = sqlQuery.value(nrQueryPos).toInt();
	mName = sqlQuery.value(nameQueryPos).toString();
	mOrt = sqlQuery.value(ortQueryPos).toString();
}


/*
 * initialize Kunde from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses own property names
 * corresponding export method: toMap()
 */
void Kunde::fillFromMap(const QVariantMap& kundeMap)
{
	mNr = kundeMap.value(nrKey).toInt();
	mName = kundeMap.value(nameKey).toString();
	mOrt = kundeMap.value(ortKey).toString();
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses foreign property names - per ex. from Server API
 * corresponding export method: toForeignMap()
 */
void Kunde::fillFromForeignMap(const QVariantMap& kundeMap)
{
	mNr = kundeMap.value(nrForeignKey).toInt();
	mName = kundeMap.value(nameForeignKey).toString();
	mOrt = kundeMap.value(ortForeignKey).toString();
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * excludes transient values
 * uses own property names
 * corresponding export method: toCacheMap()
 */
void Kunde::fillFromCacheMap(const QVariantMap& kundeMap)
{
	mNr = kundeMap.value(nrKey).toInt();
	mName = kundeMap.value(nameKey).toString();
	mOrt = kundeMap.value(ortKey).toString();
}

void Kunde::prepareNew()
{
}

/*
 * Checks if all mandatory attributes, all DomainKeys and uuid's are filled
 */
bool Kunde::isValid()
{
	if (mNr == -1) {
		return false;
	}
	return true;
}
	
/*
 * Exports Properties from Kunde as QVariantMap
 * exports ALL data including transient properties
 * To store persistent Data in JsonDataAccess use toCacheMap()
 */
QVariantMap Kunde::toMap()
{
	QVariantMap kundeMap;
	kundeMap.insert(nrKey, mNr);
	kundeMap.insert(nameKey, mName);
	kundeMap.insert(ortKey, mOrt);
	return kundeMap;
}

/*
 * Exports Properties from Kunde as QVariantMap
 * To send data as payload to Server
 * Makes it possible to use defferent naming conditions
 */
QVariantMap Kunde::toForeignMap()
{
	QVariantMap kundeMap;
	kundeMap.insert(nrForeignKey, mNr);
	kundeMap.insert(nameForeignKey, mName);
	kundeMap.insert(ortForeignKey, mOrt);
	return kundeMap;
}


/*
 * Exports Properties from Kunde as QVariantMap
 * transient properties are excluded:
 * To export ALL data use toMap()
 */
QVariantMap Kunde::toCacheMap()
{
	// no transient properties found from data model
	// use default toMao()
	return toMap();
}
// ATT 
// Mandatory: nr
// Domain KEY: nr
int Kunde::nr() const
{
	return mNr;
}

void Kunde::setNr(int nr)
{
	if (nr != mNr) {
		mNr = nr;
		emit nrChanged(nr);
	}
}
// ATT 
// Optional: name
QString Kunde::name() const
{
	return mName;
}

void Kunde::setName(QString name)
{
	if (name != mName) {
		mName = name;
		emit nameChanged(name);
	}
}
// ATT 
// Optional: ort
QString Kunde::ort() const
{
	return mOrt;
}

void Kunde::setOrt(QString ort)
{
	if (ort != mOrt) {
		mOrt = ort;
		emit ortChanged(ort);
	}
}


Kunde::~Kunde()
{
	// place cleanUp code here
}
	
