#include "KCToolServer.h"
#include <QTcpSocket>
#include "KCClient.h"

KCToolServer::KCToolServer(KCClient *client, QObject *parent):
	QTcpServer(parent), client(client)
{
	connect(this, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

KCToolServer::~KCToolServer()
{
	
}

void KCToolServer::handleRequest(QTcpSocket *socket)
{
	// Get the data out of it
	QString method = socket->property("method").toString();
	QString path = socket->property("path").toString();
	QByteArray content = socket->property("buffer").toByteArray();
	QVariantMap headers = socket->property("headers").toMap();
	
	// Hold response data
	int resCode(200);
	QString resMsg("OK");
	QString resType("text/json");
	QByteArray resBody = "{ \"success\": true }";
	
	// Handle POSTs
	if(method == "POST")
	{
		QVariant data = client->dataFromRawResponse(content);
		
		if(path == "/kcsapi/api_get_master/ship")
			client->_processMasterShipsData(data);
		else if(path == "/kcsapi/api_get_member/ship")
			client->_processPlayerShipsData(data);
		else if(path == "/kcsapi/api_get_member/deck")
			client->_processPlayerFleetsData(data);
		else if(path == "/kcsapi/api_get_member/ndock")
			client->_processPlayerRepairsData(data);
		else if(path == "/kcsapi/api_get_member/kdock")
			client->_processPlayerConstructionsData(data);
		else
		{
			//qDebug() << "Dunno what to do about" << path;
			resCode = 404;
			resMsg = "Not Found";
		}
	}
	// I might add other methods later (if I find a use), but for now, refuse them
	else
	{
		qDebug() << "Invalid Method:" << method << "(" << path << ")";
		resCode = 405;
		resMsg = "Method Not Allowed";
	}
	
	// Write out a reply to the client
	reply(socket, resCode, resMsg, resType, (resBody.size() > 0 ? resBody : resMsg.toUtf8()));
	
	// If it's not a Connection: close request, reset the state properties
	if(headers.value("Connection").toString() != "close")
	{
		socket->setProperty("firstLineRead", QVariant());
		socket->setProperty("headerRead", QVariant());
		socket->setProperty("method", QVariant());
		socket->setProperty("path", QVariant());
		socket->setProperty("buffer", QVariant());
		socket->setProperty("headers", QVariant());
	}
	// Otherwise, close it behind us
	else
		socket->close();
}

void KCToolServer::reply(QTcpSocket *socket, int code, QString message, QString contentType, QByteArray body)
{
	socket->write(QString("HTTP/1.1 %1 %2\r\n").arg(QString::number(code), message).toUtf8());
	socket->write(QString("Date: %1\r\n").arg(QDateTime::currentDateTimeUtc().toString("ddd, d MMM yyyy hh:mm:ss")).toUtf8());
	socket->write(QString("Content-Length: %1\r\n").arg(QString::number(body.size())).toUtf8());
	socket->write(QString("Content-Type: %1\r\n").arg(contentType).toUtf8());
	socket->write(QString("\r\n").toUtf8());
	socket->write(body);
}

void KCToolServer::onNewConnection()
{
	while(this->hasPendingConnections())
	{
		QTcpSocket *socket = this->nextPendingConnection();
		connect(socket, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead()));
	}
}

void KCToolServer::onSocketReadyRead()
{
	QTcpSocket *socket = qobject_cast<QTcpSocket*>(QObject::sender());
	
	// Parse the first line
	if(!socket->property("firstLineRead").toBool())
	{
		QString line(socket->readLine());
		int sepPos1(line.indexOf(" "));
		int sepPos2(line.indexOf(" ", sepPos1+1));
		QString method(line.left(sepPos1));
		QString path(line.mid(sepPos1+1, sepPos2 - sepPos1 - 1));
		socket->setProperty("method", method);
		socket->setProperty("path", path);
		socket->setProperty("firstLineRead", true);
	}
	
	// Parse Headers!
	int contentLength = -1;
	if(!socket->property("headerRead").toBool())
	{
		QVariantMap headers(socket->property("headers").toMap());
		
		while(socket->canReadLine())
		{
			QString line = QString(socket->readLine()).trimmed();
			
			// The header section is terminated by an empty line
			if(line == "")
			{
				socket->setProperty("headerRead", true);
				break;
			}
			
			// Split it up
			int sepPos(line.indexOf(":"));
			QString key(line.left(sepPos).trimmed());
			QString val(line.mid(sepPos+1).trimmed());
			headers.insertMulti(key, val);
			
			// Check for the Content-Length header
			if(key == "Content-Length")
				contentLength = val.toInt();
		}
		
		socket->setProperty("headers", headers);
	}
	
	// Read the body into a buffer
	if(socket->bytesAvailable())
	{
		QByteArray buffer(socket->property("buffer").toByteArray());
		buffer.append(socket->readAll());
		socket->setProperty("buffer", buffer);
		
		// If this is a later chunk, check the old Content-Length header
		if(contentLength == -1)
			contentLength = socket->property("headers").toMap().value("Content-Length").toInt();
		
		// If we have a Content-Length (toLong() fails with 0)
		if(contentLength > 0 && buffer.size() >= contentLength)
		{
			this->handleRequest(socket);
			socket->close();
		}
	}
	else if(contentLength == -1 || contentLength == 0)
	{
		this->handleRequest(socket);
		socket->close();
	}
}