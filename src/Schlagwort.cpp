#include "Schlagwort.hpp"
#include <QDebug>
#include <quuid.h>

// keys of QVariantMap used in this APP
static const QString uuidKey = "uuid";
static const QString textKey = "text";

// keys used from Server API etc
static const QString uuidForeignKey = "uuid";
static const QString textForeignKey = "text";

/*
 * Default Constructor if Schlagwort not initialized from QVariantMap
 */
Schlagwort::Schlagwort(QObject *parent) :
        QObject(parent), mUuid(""), mText("")
{
}

/*
 * initialize Schlagwort from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses own property names
 * corresponding export method: toMap()
 */
void Schlagwort::fillFromMap(const QVariantMap& schlagwortMap)
{
	mUuid = schlagwortMap.value(uuidKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mText = schlagwortMap.value(textKey).toString();
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses foreign property names - per ex. from Server API
 * corresponding export method: toForeignMap()
 */
void Schlagwort::fillFromForeignMap(const QVariantMap& schlagwortMap)
{
	mUuid = schlagwortMap.value(uuidForeignKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mText = schlagwortMap.value(textForeignKey).toString();
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * excludes transient values
 * uses own property names
 * corresponding export method: toCacheMap()
 */
void Schlagwort::fillFromCacheMap(const QVariantMap& schlagwortMap)
{
	mUuid = schlagwortMap.value(uuidKey).toString();
	if (mUuid.isEmpty()) {
		mUuid = QUuid::createUuid().toString();
		mUuid = mUuid.right(mUuid.length() - 1);
		mUuid = mUuid.left(mUuid.length() - 1);
	}	
	mText = schlagwortMap.value(textKey).toString();
}

void Schlagwort::prepareNew()
{
	mUuid = QUuid::createUuid().toString();
	mUuid = mUuid.right(mUuid.length() - 1);
	mUuid = mUuid.left(mUuid.length() - 1);
}

/*
 * Checks if all mandatory attributes, all DomainKeys and uuid's are filled
 */
bool Schlagwort::isValid()
{
	if (mUuid.isNull() || mUuid.isEmpty()) {
		return false;
	}
	return true;
}
	
/*
 * Exports Properties from Schlagwort as QVariantMap
 * exports ALL data including transient properties
 * To store persistent Data in JsonDataAccess use toCacheMap()
 */
QVariantMap Schlagwort::toMap()
{
	QVariantMap schlagwortMap;
	schlagwortMap.insert(uuidKey, mUuid);
	schlagwortMap.insert(textKey, mText);
	return schlagwortMap;
}

/*
 * Exports Properties from Schlagwort as QVariantMap
 * To send data as payload to Server
 * Makes it possible to use defferent naming conditions
 */
QVariantMap Schlagwort::toForeignMap()
{
	QVariantMap schlagwortMap;
	schlagwortMap.insert(uuidForeignKey, mUuid);
	schlagwortMap.insert(textForeignKey, mText);
	return schlagwortMap;
}


/*
 * Exports Properties from Schlagwort as QVariantMap
 * transient properties are excluded:
 * To export ALL data use toMap()
 */
QVariantMap Schlagwort::toCacheMap()
{
	// no transient properties found from data model
	// use default toMao()
	return toMap();
}
// ATT 
// Mandatory: uuid
// Domain KEY: uuid
QString Schlagwort::uuid() const
{
	return mUuid;
}

void Schlagwort::setUuid(QString uuid)
{
	if (uuid != mUuid) {
		mUuid = uuid;
		emit uuidChanged(uuid);
	}
}
// ATT 
// Optional: text
QString Schlagwort::text() const
{
	return mText;
}

void Schlagwort::setText(QString text)
{
	if (text != mText) {
		mText = text;
		emit textChanged(text);
	}
}


Schlagwort::~Schlagwort()
{
	// place cleanUp code here
}
	
