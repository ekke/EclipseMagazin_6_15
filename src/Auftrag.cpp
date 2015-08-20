#include "Auftrag.hpp"
#include <QDebug>
#include <quuid.h>

// keys of QVariantMap used in this APP
static const QString nrKey = "nr";
static const QString datumKey = "datum";
static const QString bemerkungKey = "bemerkung";
static const QString positionenKey = "positionen";
static const QString tagsKey = "tags";
static const QString auftraggeberKey = "auftraggeber";

// keys used from Server API etc
static const QString nrForeignKey = "nr";
static const QString datumForeignKey = "datum";
static const QString bemerkungForeignKey = "bemerkung";
static const QString positionenForeignKey = "positionen";
static const QString tagsForeignKey = "tags";
static const QString auftraggeberForeignKey = "auftraggeber";

/*
 * Default Constructor if Auftrag not initialized from QVariantMap
 */
Auftrag::Auftrag(QObject *parent) :
        QObject(parent), mNr(-1), mBemerkung("")
{
	// lazy references:
	mAuftraggeber = -1;
	mAuftraggeberAsDataObject = 0;
	mAuftraggeberInvalid = false;
	// Date, Time or Timestamp ? construct null value
	mDatum = QDate();
		// lazy Arrays where only keys are persisted
		mTagsKeysResolved = false;
}

bool Auftrag::isAllResolved()
{
	if (hasAuftraggeber() && !isAuftraggeberResolvedAsDataObject()) {
		return false;
	}
    if(!areTagsKeysResolved()) {
        return false;
    }
    return true;
}

/*
 * initialize Auftrag from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses own property names
 * corresponding export method: toMap()
 */
void Auftrag::fillFromMap(const QVariantMap& auftragMap)
{
	mNr = auftragMap.value(nrKey).toInt();
	if (auftragMap.contains(datumKey)) {
		// always getting the Date as a String (from server or JSON)
		QString datumAsString = auftragMap.value(datumKey).toString();
		mDatum = QDate::fromString(datumAsString, "yyyy-MM-dd");
		if (!mDatum.isValid()) {
			mDatum = QDate();
			qDebug() << "mDatum is not valid for String: " << datumAsString;
		}
	}
	mBemerkung = auftragMap.value(bemerkungKey).toString();
	// auftraggeber lazy pointing to Kunde* (domainKey: nr)
	if (auftragMap.contains(auftraggeberKey)) {
		mAuftraggeber = auftragMap.value(auftraggeberKey).toInt();
		if (mAuftraggeber != -1) {
			// resolve the corresponding Data Object on demand from DataManager
		}
	}
	// mPositionen is List of Position*
	QVariantList positionenList;
	positionenList = auftragMap.value(positionenKey).toList();
	mPositionen.clear();
	for (int i = 0; i < positionenList.size(); ++i) {
		QVariantMap positionenMap;
		positionenMap = positionenList.at(i).toMap();
		Position* position = new Position();
		position->setParent(this);
		position->fillFromMap(positionenMap);
		mPositionen.append(position);
	}
	// mTags is (lazy loaded) Array of Schlagwort*
	mTagsKeys = auftragMap.value(tagsKey).toStringList();
	// mTags must be resolved later if there are keys
	mTagsKeysResolved = (mTagsKeys.size() == 0);
	mTags.clear();
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * includes also transient values
 * uses foreign property names - per ex. from Server API
 * corresponding export method: toForeignMap()
 */
void Auftrag::fillFromForeignMap(const QVariantMap& auftragMap)
{
	mNr = auftragMap.value(nrForeignKey).toInt();
	if (auftragMap.contains(datumForeignKey)) {
		// always getting the Date as a String (from server or JSON)
		QString datumAsString = auftragMap.value(datumForeignKey).toString();
		mDatum = QDate::fromString(datumAsString, "yyyy-MM-dd");
		if (!mDatum.isValid()) {
			mDatum = QDate();
			qDebug() << "mDatum is not valid for String: " << datumAsString;
		}
	}
	mBemerkung = auftragMap.value(bemerkungForeignKey).toString();
	// auftraggeber lazy pointing to Kunde* (domainKey: nr)
	if (auftragMap.contains(auftraggeberForeignKey)) {
		mAuftraggeber = auftragMap.value(auftraggeberForeignKey).toInt();
		if (mAuftraggeber != -1) {
			// resolve the corresponding Data Object on demand from DataManager
		}
	}
	// mPositionen is List of Position*
	QVariantList positionenList;
	positionenList = auftragMap.value(positionenForeignKey).toList();
	mPositionen.clear();
	for (int i = 0; i < positionenList.size(); ++i) {
		QVariantMap positionenMap;
		positionenMap = positionenList.at(i).toMap();
		Position* position = new Position();
		position->setParent(this);
		position->fillFromForeignMap(positionenMap);
		mPositionen.append(position);
	}
	// mTags is (lazy loaded) Array of Schlagwort*
	mTagsKeys = auftragMap.value(tagsForeignKey).toStringList();
	// mTags must be resolved later if there are keys
	mTagsKeysResolved = (mTagsKeys.size() == 0);
	mTags.clear();
}
/*
 * initialize OrderData from QVariantMap
 * Map got from JsonDataAccess or so
 * excludes transient values
 * uses own property names
 * corresponding export method: toCacheMap()
 */
void Auftrag::fillFromCacheMap(const QVariantMap& auftragMap)
{
	mNr = auftragMap.value(nrKey).toInt();
	if (auftragMap.contains(datumKey)) {
		// always getting the Date as a String (from server or JSON)
		QString datumAsString = auftragMap.value(datumKey).toString();
		mDatum = QDate::fromString(datumAsString, "yyyy-MM-dd");
		if (!mDatum.isValid()) {
			mDatum = QDate();
			qDebug() << "mDatum is not valid for String: " << datumAsString;
		}
	}
	mBemerkung = auftragMap.value(bemerkungKey).toString();
	// auftraggeber lazy pointing to Kunde* (domainKey: nr)
	if (auftragMap.contains(auftraggeberKey)) {
		mAuftraggeber = auftragMap.value(auftraggeberKey).toInt();
		if (mAuftraggeber != -1) {
			// resolve the corresponding Data Object on demand from DataManager
		}
	}
	// mPositionen is List of Position*
	QVariantList positionenList;
	positionenList = auftragMap.value(positionenKey).toList();
	mPositionen.clear();
	for (int i = 0; i < positionenList.size(); ++i) {
		QVariantMap positionenMap;
		positionenMap = positionenList.at(i).toMap();
		Position* position = new Position();
		position->setParent(this);
		position->fillFromCacheMap(positionenMap);
		mPositionen.append(position);
	}
	// mTags is (lazy loaded) Array of Schlagwort*
	mTagsKeys = auftragMap.value(tagsKey).toStringList();
	// mTags must be resolved later if there are keys
	mTagsKeysResolved = (mTagsKeys.size() == 0);
	mTags.clear();
}

void Auftrag::prepareNew()
{
}

/*
 * Checks if all mandatory attributes, all DomainKeys and uuid's are filled
 */
bool Auftrag::isValid()
{
	if (mNr == -1) {
		return false;
	}
	if (mPositionen.size() == 0) {
		return false;
	}
	// auftraggeber lazy pointing to Kunde* (domainKey: nr)
	if (mAuftraggeber == -1) {
		return false;
	}
	return true;
}
	
/*
 * Exports Properties from Auftrag as QVariantMap
 * exports ALL data including transient properties
 * To store persistent Data in JsonDataAccess use toCacheMap()
 */
QVariantMap Auftrag::toMap()
{
	QVariantMap auftragMap;
	// auftraggeber lazy pointing to Kunde* (domainKey: nr)
	if (mAuftraggeber != -1) {
		auftragMap.insert(auftraggeberKey, mAuftraggeber);
	}
	// mTags points to Schlagwort*
	// lazy array: persist only keys
	if(mTagsKeysResolved || (mTagsKeys.size() == 0 && mTags.size() != 0)) {
		mTagsKeys.clear();
		for (int i = 0; i < mTags.size(); ++i) {
			Schlagwort* schlagwort;
			schlagwort = mTags.at(i);
			mTagsKeys << schlagwort->uuid();
		}
	}
	auftragMap.insert(tagsKey, mTagsKeys);
	auftragMap.insert(nrKey, mNr);
	if (hasDatum()) {
		auftragMap.insert(datumKey, mDatum.toString("yyyy-MM-dd"));
	}
	auftragMap.insert(bemerkungKey, mBemerkung);
	// mPositionen points to Position*
	auftragMap.insert(positionenKey, positionenAsQVariantList());
	return auftragMap;
}

/*
 * Exports Properties from Auftrag as QVariantMap
 * To send data as payload to Server
 * Makes it possible to use defferent naming conditions
 */
QVariantMap Auftrag::toForeignMap()
{
	QVariantMap auftragMap;
	// auftraggeber lazy pointing to Kunde* (domainKey: nr)
	if (mAuftraggeber != -1) {
		auftragMap.insert(auftraggeberForeignKey, mAuftraggeber);
	}
	// mTags points to Schlagwort*
	// lazy array: persist only keys
	if(mTagsKeysResolved || (mTagsKeys.size() == 0 && mTags.size() != 0)) {
		mTagsKeys.clear();
		for (int i = 0; i < mTags.size(); ++i) {
			Schlagwort* schlagwort;
			schlagwort = mTags.at(i);
			mTagsKeys << schlagwort->uuid();
		}
	}
	auftragMap.insert(tagsKey, mTagsKeys);
	auftragMap.insert(nrForeignKey, mNr);
	if (hasDatum()) {
		auftragMap.insert(datumForeignKey, mDatum.toString("yyyy-MM-dd"));
	}
	auftragMap.insert(bemerkungForeignKey, mBemerkung);
	// mPositionen points to Position*
	auftragMap.insert(positionenForeignKey, positionenAsQVariantList());
	return auftragMap;
}


/*
 * Exports Properties from Auftrag as QVariantMap
 * transient properties are excluded:
 * To export ALL data use toMap()
 */
QVariantMap Auftrag::toCacheMap()
{
	// no transient properties found from data model
	// use default toMao()
	return toMap();
}
// REF
// Lazy: auftraggeber
// Mandatory: auftraggeber
// auftraggeber lazy pointing to Kunde* (domainKey: nr)
int Auftrag::auftraggeber() const
{
	return mAuftraggeber;
}
Kunde* Auftrag::auftraggeberAsDataObject() const
{
	return mAuftraggeberAsDataObject;
}
void Auftrag::setAuftraggeber(int auftraggeber)
{
	if (auftraggeber != mAuftraggeber) {
        // remove old Data Object if one was resolved
        if (mAuftraggeberAsDataObject) {
            // reset pointer, don't delete the independent object !
            mAuftraggeberAsDataObject = 0;
        }
        // set the new lazy reference
        mAuftraggeber = auftraggeber;
        mAuftraggeberInvalid = false;
        emit auftraggeberChanged(auftraggeber);
        if (auftraggeber != -1) {
            // resolve the corresponding Data Object on demand from DataManager
        }
    }
}
void Auftrag::removeAuftraggeber()
{
	if (mAuftraggeber != -1) {
		setAuftraggeber(-1);
	}
}
bool Auftrag::hasAuftraggeber()
{
    if (!mAuftraggeberInvalid && mAuftraggeber != -1) {
        return true;
    } else {
        return false;
    }
}
bool Auftrag::isAuftraggeberResolvedAsDataObject()
{
    if (!mAuftraggeberInvalid && mAuftraggeberAsDataObject) {
        return true;
    } else {
        return false;
    }
}

// lazy bound Data Object was resolved. overwrite nr if different
void Auftrag::resolveAuftraggeberAsDataObject(Kunde* kunde)
{
    if (kunde) {
        if (kunde->nr() != mAuftraggeber) {
            setAuftraggeber(kunde->nr());
        }
        mAuftraggeberAsDataObject = kunde;
        mAuftraggeberInvalid = false;
    }
}
void Auftrag::markAuftraggeberAsInvalid()
{
    mAuftraggeberInvalid = true;
}
// ATT 
// Mandatory: nr
// Domain KEY: nr
int Auftrag::nr() const
{
	return mNr;
}

void Auftrag::setNr(int nr)
{
	if (nr != mNr) {
		mNr = nr;
		emit nrChanged(nr);
	}
}
// ATT 
// Optional: datum
/**
 * in QML set DateTimePicker value this way:
 * myPicker.value = new Date(datum)
 */
QDate Auftrag::datum() const
{
	return mDatum;
}

/**
 * from QML DateTime Picker use as parameter:
 * datum = new Date(myPicker.value)
 */
void Auftrag::setDatum(QDate datum)
{
	if (datum != mDatum) {
		mDatum = datum;
		emit datumChanged(datum);
	}
}
bool Auftrag::hasDatum()
{
	return !mDatum.isNull() && mDatum.isValid();
}
// ATT 
// Optional: bemerkung
QString Auftrag::bemerkung() const
{
	return mBemerkung;
}

void Auftrag::setBemerkung(QString bemerkung)
{
	if (bemerkung != mBemerkung) {
		mBemerkung = bemerkung;
		emit bemerkungChanged(bemerkung);
	}
}
// REF
// Opposite: auftragsKopf
// Mandatory: positionen
QVariantList Auftrag::positionenAsQVariantList()
{
	QVariantList positionenList;
	for (int i = 0; i < mPositionen.size(); ++i) {
        positionenList.append((mPositionen.at(i))->toMap());
    }
	return positionenList;
}
/**
 * creates a new Position
 * parent is this Auftrag
 * if data is successfully entered you must INVOKE addToPositionen()
 * if edit was canceled you must undoCreateElementOfPositionen to free up memory
 */
Position* Auftrag::createElementOfPositionen()
{
    Position* position;
    position = new Position();
    position->setParent(this);
    position->prepareNew();
    return position;
}

/**
 * if createElementOfPositionen was canceled from UI
 * this method deletes the Object of Type Position
 * 
 * to delete a allready into positionen inserted  Position
 * you must use removeFromPositionen
 */
void Auftrag::undoCreateElementOfPositionen(Position* position)
{
    if (position) {
        position->deleteLater();
        position = 0;
    }
}
void Auftrag::addToPositionen(Position* position)
{
    mPositionen.append(position);
    emit addedToPositionen(position);
}

bool Auftrag::removeFromPositionen(Position* position)
{
    bool ok = false;
    ok = mPositionen.removeOne(position);
    if (!ok) {
    	qDebug() << "Position* not found in positionen";
    	return false;
    }
    emit removedFromPositionenByUuid(position->uuid());
    // positionen are contained - so we must delete them
    position->deleteLater();
    position = 0;
    return true;
}
void Auftrag::clearPositionen()
{
    for (int i = mPositionen.size(); i > 0; --i) {
        removeFromPositionen(mPositionen.last());
    }
}
void Auftrag::addToPositionenFromMap(const QVariantMap& positionMap)
{
    Position* position = new Position();
    position->setParent(this);
    position->fillFromMap(positionMap);
    mPositionen.append(position);
    emit addedToPositionen(position);
}
bool Auftrag::removeFromPositionenByUuid(const QString& uuid)
{
    for (int i = 0; i < mPositionen.size(); ++i) {
    	Position* position;
        position = mPositionen.at(i);
        if (position->uuid() == uuid) {
        	mPositionen.removeAt(i);
        	emit removedFromPositionenByUuid(uuid);
        	// positionen are contained - so we must delete them
        	position->deleteLater();
        	position = 0;
        	return true;
        }
    }
    qDebug() << "uuid not found in positionen: " << uuid;
    return false;
}

int Auftrag::positionenCount()
{
    return mPositionen.size();
}
QList<Position*> Auftrag::positionen()
{
	return mPositionen;
}
void Auftrag::setPositionen(QList<Position*> positionen) 
{
	if (positionen != mPositionen) {
		mPositionen = positionen;
		emit positionenChanged(positionen);
	}
}
/**
 * to access lists from QML we're using QDeclarativeListProperty
 * and implement methods to append, count and clear
 * now from QML we can use
 * auftrag.positionenPropertyList.length to get the size
 * auftrag.positionenPropertyList[2] to get Position* at position 2
 * auftrag.positionenPropertyList = [] to clear the list
 * or get easy access to properties like
 * auftrag.positionenPropertyList[2].myPropertyName
 */
QDeclarativeListProperty<Position> Auftrag::positionenPropertyList()
{
    return QDeclarativeListProperty<Position>(this, 0, &Auftrag::appendToPositionenProperty,
            &Auftrag::positionenPropertyCount, &Auftrag::atPositionenProperty,
            &Auftrag::clearPositionenProperty);
}
void Auftrag::appendToPositionenProperty(QDeclarativeListProperty<Position> *positionenList,
        Position* position)
{
    Auftrag *auftragObject = qobject_cast<Auftrag *>(positionenList->object);
    if (auftragObject) {
		position->setParent(auftragObject);
        auftragObject->mPositionen.append(position);
        emit auftragObject->addedToPositionen(position);
    } else {
        qWarning() << "cannot append Position* to positionen " << "Object is not of type Auftrag*";
    }
}
int Auftrag::positionenPropertyCount(QDeclarativeListProperty<Position> *positionenList)
{
    Auftrag *auftrag = qobject_cast<Auftrag *>(positionenList->object);
    if (auftrag) {
        return auftrag->mPositionen.size();
    } else {
        qWarning() << "cannot get size positionen " << "Object is not of type Auftrag*";
    }
    return 0;
}
Position* Auftrag::atPositionenProperty(QDeclarativeListProperty<Position> *positionenList, int pos)
{
    Auftrag *auftrag = qobject_cast<Auftrag *>(positionenList->object);
    if (auftrag) {
        if (auftrag->mPositionen.size() > pos) {
            return auftrag->mPositionen.at(pos);
        }
        qWarning() << "cannot get Position* at pos " << pos << " size is "
                << auftrag->mPositionen.size();
    } else {
        qWarning() << "cannot get Position* at pos " << pos << "Object is not of type Auftrag*";
    }
    return 0;
}
void Auftrag::clearPositionenProperty(QDeclarativeListProperty<Position> *positionenList)
{
    Auftrag *auftrag = qobject_cast<Auftrag *>(positionenList->object);
    if (auftrag) {
        // positionen are contained - so we must delete them
        for (int i = 0; i < auftrag->mPositionen.size(); ++i) {
            auftrag->mPositionen.at(i)->deleteLater();
        }
        auftrag->mPositionen.clear();
    } else {
        qWarning() << "cannot clear positionen " << "Object is not of type Auftrag*";
    }
}
// ATT 
// Optional: tags
QVariantList Auftrag::tagsAsQVariantList()
{
	QVariantList tagsList;
	for (int i = 0; i < mTags.size(); ++i) {
        tagsList.append((mTags.at(i))->toMap());
    }
	return tagsList;
}
// no create() or undoCreate() because dto is root object
// see methods in DataManager
void Auftrag::addToTags(Schlagwort* schlagwort)
{
    mTags.append(schlagwort);
    emit addedToTags(schlagwort);
}

bool Auftrag::removeFromTags(Schlagwort* schlagwort)
{
    bool ok = false;
    ok = mTags.removeOne(schlagwort);
    if (!ok) {
    	qDebug() << "Schlagwort* not found in tags";
    	return false;
    }
    // tags are independent - DON'T delete them
    return true;
}
void Auftrag::clearTags()
{
    for (int i = mTags.size(); i > 0; --i) {
        removeFromTags(mTags.last());
    }
}

/**
 * lazy Array of independent Data Objects: only keys are persited
 * so we get a list of keys (uuid or domain keys) from map
 * and we persist only the keys toMap()
 * after initializing the keys must be resolved:
 * - get the list of keys: tagsKeys()
 * - resolve them from DataManager
 * - then resolveTagsKeys()
 */
bool Auftrag::areTagsKeysResolved()
{
    return mTagsKeysResolved;
}

QStringList Auftrag::tagsKeys()
{
    return mTagsKeys;
}

void Auftrag::resolveTagsKeys(QList<Schlagwort*> tags)
{
    if(mTagsKeysResolved){
        return;
    }
    mTags.clear();
    for (int i = 0; i < tags.size(); ++i) {
        addToTags(tags.at(i));
    }
    mTagsKeysResolved = true;
}

int Auftrag::tagsCount()
{
    return mTags.size();
}
QList<Schlagwort*> Auftrag::tags()
{
	return mTags;
}
void Auftrag::setTags(QList<Schlagwort*> tags) 
{
	if (tags != mTags) {
		mTags = tags;
		emit tagsChanged(tags);
	}
}
/**
 * to access lists from QML we're using QDeclarativeListProperty
 * and implement methods to append, count and clear
 * now from QML we can use
 * auftrag.tagsPropertyList.length to get the size
 * auftrag.tagsPropertyList[2] to get Schlagwort* at position 2
 * auftrag.tagsPropertyList = [] to clear the list
 * or get easy access to properties like
 * auftrag.tagsPropertyList[2].myPropertyName
 */
QDeclarativeListProperty<Schlagwort> Auftrag::tagsPropertyList()
{
    return QDeclarativeListProperty<Schlagwort>(this, 0, &Auftrag::appendToTagsProperty,
            &Auftrag::tagsPropertyCount, &Auftrag::atTagsProperty,
            &Auftrag::clearTagsProperty);
}
void Auftrag::appendToTagsProperty(QDeclarativeListProperty<Schlagwort> *tagsList,
        Schlagwort* schlagwort)
{
    Auftrag *auftragObject = qobject_cast<Auftrag *>(tagsList->object);
    if (auftragObject) {
        auftragObject->mTags.append(schlagwort);
        emit auftragObject->addedToTags(schlagwort);
    } else {
        qWarning() << "cannot append Schlagwort* to tags " << "Object is not of type Auftrag*";
    }
}
int Auftrag::tagsPropertyCount(QDeclarativeListProperty<Schlagwort> *tagsList)
{
    Auftrag *auftrag = qobject_cast<Auftrag *>(tagsList->object);
    if (auftrag) {
        return auftrag->mTags.size();
    } else {
        qWarning() << "cannot get size tags " << "Object is not of type Auftrag*";
    }
    return 0;
}
Schlagwort* Auftrag::atTagsProperty(QDeclarativeListProperty<Schlagwort> *tagsList, int pos)
{
    Auftrag *auftrag = qobject_cast<Auftrag *>(tagsList->object);
    if (auftrag) {
        if (auftrag->mTags.size() > pos) {
            return auftrag->mTags.at(pos);
        }
        qWarning() << "cannot get Schlagwort* at pos " << pos << " size is "
                << auftrag->mTags.size();
    } else {
        qWarning() << "cannot get Schlagwort* at pos " << pos << "Object is not of type Auftrag*";
    }
    return 0;
}
void Auftrag::clearTagsProperty(QDeclarativeListProperty<Schlagwort> *tagsList)
{
    Auftrag *auftrag = qobject_cast<Auftrag *>(tagsList->object);
    if (auftrag) {
        // tags are independent - DON'T delete them
        auftrag->mTags.clear();
    } else {
        qWarning() << "cannot clear tags " << "Object is not of type Auftrag*";
    }
}


Auftrag::~Auftrag()
{
	// place cleanUp code here
}
	
