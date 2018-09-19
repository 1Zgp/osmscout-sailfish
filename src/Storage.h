/*
  OSMScout for SFOS
  Copyright (C) 2018 Lukas Karas

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef OSMSCOUT_SAILFISH_STORAGE_H
#define OSMSCOUT_SAILFISH_STORAGE_H

#include <osmscout/gpx/Track.h>
#include <osmscout/gpx/Waypoint.h>
#include <osmscout/gpx/Utils.h>
#include <osmscout/gpx/GpxFile.h>
#include <osmscout/util/GeoBox.h>

#include <QObject>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QDir>
#include <QtCore/QDateTime>

#include <atomic>

class ErrorCallback: public QObject, public osmscout::gpx::ProcessCallback
{
  Q_OBJECT
  Q_DISABLE_COPY(ErrorCallback)

signals:
  void error(QString);

public:
  ErrorCallback() = default;
  virtual ~ErrorCallback() = default;

  virtual void Error(std::string error);
};

class TrackStatistics
{
public:
  TrackStatistics() = default;

  TrackStatistics(const osmscout::Distance &distance,
                  const osmscout::GeoBox &bbox):
    distance(distance), bbox(bbox)
  {};

  TrackStatistics(const TrackStatistics &o):
    distance(o.distance), bbox(o.bbox)
  {};

public:
  osmscout::Distance distance;
  osmscout::GeoBox   bbox;
};

class Track
{
public:
  Track() = default;
  Track(qint64 id, qint64 collectionId,  QString name,  QString description, bool open,
        const QDateTime &creationTime, const QDateTime &lastModification,
        const osmscout::Distance &distance,
        const osmscout::GeoBox &bbox):
    id(id), collectionId(collectionId), name(name), description(description), open(open),
    creationTime(creationTime), lastModification(lastModification),
    statistics(distance, bbox)
  {};

  Track(const Track &o):
    id(o.id), collectionId(o.collectionId), name(o.name), description(o.description), open(o.open),
    creationTime(o.creationTime), lastModification(o.lastModification),
    statistics(o.statistics), data(o.data)
  {};

public:
  qint64 id{-1};
  qint64 collectionId{-1};
  QString name;
  QString description;
  bool open{false};
  QDateTime creationTime;
  QDateTime lastModification;

  TrackStatistics statistics;
  std::shared_ptr<osmscout::gpx::Track> data;
};

class Waypoint
{
public:
  Waypoint() = default;

  Waypoint(qint64 id, const QDateTime &lastModification, const osmscout::gpx::Waypoint &data):
    id(id), lastModification(lastModification), data(data)
  {}

  Waypoint(qint64 id, const QDateTime &lastModification, osmscout::gpx::Waypoint &&data):
    id(id), lastModification(lastModification), data(std::move(data))
  {}

public:
  qint64 id{-1};
  QDateTime lastModification;
  osmscout::gpx::Waypoint data{osmscout::GeoCoord()};
};

class Collection
{
public:
  Collection() = default;
  Collection(qint64 id):
    id(id)
  {};

  Collection(qint64 id,
             bool visible,
             const QString &name,
             const QString &description):
    id(id), visible(visible), name(name), description(description)
  {};

public:
  qint64 id{-1};
  bool visible{false};
  QString name;
  QString description;

  std::shared_ptr<std::vector<Track>> tracks;
  std::shared_ptr<std::vector<Waypoint>> waypoints;
};

class Storage : public QObject{
  Q_OBJECT
  Q_DISABLE_COPY(Storage)

private:
  bool updateSchema();

signals:
  void initialised();
  void initialisationError(QString error);

  void collectionsLoaded(std::vector<Collection> collections, bool ok);
  void collectionDetailsLoaded(Collection collection, bool ok);
  void trackDataLoaded(Track track, bool complete, bool ok);
  void collectionExported(bool success);
  void error(QString);

public slots:
  void init();

  /**
   * load collection list
   * emits collectionsLoaded
   */
  void loadCollections();

  /**
   * load list of tracks and waypoints
   * emits collectionDetailsLoaded
   */
  void loadCollectionDetails(Collection collection);

  /**
   * load track data
   * emits trackDataLoaded
   */
  void loadTrackData(Track track);

  /**
   * update collection or create it (if id < 0)
   * emits collectionsLoaded signal
   */
  void updateOrCreateCollection(Collection collection);

  /**
   * delete collection
   * emits collectionsLoaded signal
   */
  void deleteCollection(qint64 id);

  /**
   * import collection from gpx file
   * emits collectionsLoaded signal
   */
  void importCollection(QString filePath);

  /**
   * delete waypoint
   * emits collectionDetailsLoaded
   */
  void deleteWaypoint(qint64 collectionId, qint64 waypointId);

  /**
   * delete waypoint
   * emits collectionDetailsLoaded
   */
  void deleteTrack(qint64 collectionId, qint64 trackId);

  /**
   * create waypoint
   * emits collectionDetailsLoaded
   */
  void createWaypoint(qint64 collectionId, double lat, double lon, QString name, QString description);

  /**
   * edit waypoint
   * emits collectionDetailsLoaded
   */
  void editWaypoint(qint64 collectionId, qint64 id, QString name, QString description);

  /**
   * edit track
   * emits collectionDetailsLoaded
   */
  void editTrack(qint64 collectionId, qint64 id, QString name, QString description);

  /**
   * emits collectionExported
   */
  void exportCollection(qint64 collectionId, QString file);

public:
  Storage(QThread *thread,
          const QDir &directory);
  virtual ~Storage();

  operator bool() const;

  static void initInstance(const QDir &directory);
  static Storage* getInstance();
  static void clearInstance();

private:
  Track makeTrack(QSqlQuery &sqlTrack) const;
  std::shared_ptr<std::vector<Track>> loadTracks(qint64 collectionId);
  std::shared_ptr<std::vector<Waypoint>> loadWaypoints(qint64 collectionId);
  void loadTrackPoints(qint64 segmentId, osmscout::gpx::TrackSegment &segment);
  bool checkAccess(QString slotName, bool requireOpen = true);
  bool importWaypoints(const osmscout::gpx::GpxFile &file, qint64 collectionId);
  bool importTracks(const osmscout::gpx::GpxFile &file, qint64 collectionId);
  bool importTrackPoints(const std::vector<osmscout::gpx::TrackPoint> &points, qint64 segId);
  TrackStatistics computeTrackStatistics(const osmscout::gpx::Track &trk) const;
  bool loadCollectionDetailsPrivate(Collection &collection);
  bool loadTrackDataPrivate(Track &track);

private :
  QSqlDatabase db;
  QThread *thread;
  QDir directory;
  std::atomic_bool ok{false};
};

#endif //OSMSCOUT_SAILFISH_STORAGE_H
