#ifndef WEBIMAGE_H
#define WEBIMAGE_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QGLWidget>
#include <QPainter>

class WebImage : public QObject
{
    Q_OBJECT

private:

    QNetworkAccessManager *sourceCodeManager, *imageManager;

    void generateFromImage()
    {
        if( image.isNull() ) return;

        glBindTexture( GL_TEXTURE_2D, textureId);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexImage2D( GL_TEXTURE_2D, 0, 3, image.width(), image.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, image.bits() );
    }

public:

    GLuint textureId;
    QImage image;

    WebImage( QObject *parent = 0 ) : QObject( parent ),
                                      sourceCodeManager( new QNetworkAccessManager( this ) ),
                                      imageManager( new QNetworkAccessManager( this ) ),
                                      textureId( 0 )
    {
        glGenTextures( 1, &textureId);
        connect( sourceCodeManager, SIGNAL( finished( QNetworkReply* ) ),  SLOT( slot_sourceCodeDownloaded( QNetworkReply* ) ) );
        connect( imageManager, SIGNAL( finished( QNetworkReply* ) ), SLOT( slot_imageDownloaded( QNetworkReply* ) ) );
    }

    void loadFromLoalImage( QString imageUri )
    {
        image.load( imageUri );
        if( !image.isNull() )
        {
            generateFromImage();
            emit imageReady( textureId );
        }
    }

    void searchForAllImages( QString search )
    {
        sourceCodeManager->get( QNetworkRequest( QUrl( "https://www.bing.com/images/search?q=" + search ) ) );
    }

    void searchForStreetViewImage( QString search )
    {
        imageManager->get( QNetworkRequest( QUrl(
        "http://maps.googleapis.com/maps/api/streetview?size=400x400&sensor=false&location=" + search ) ) );
    }

    void drawMusicInfo( QString artist, QString title )
    {
        if( image.isNull() ) return;

        QPainter musicInfoPainter( &image );
        musicInfoPainter.fillRect( 0, image.height() - 100, image.width(), 100, QColor( 0, 0, 0, 200 ) );
        musicInfoPainter.setPen( QColor( 255, 255, 255 ) );
        musicInfoPainter.setFont( QFont( "Arial", image.height() / 20 ) );
        musicInfoPainter.drawText( image.width() / 100, image.height() - image.height() / 10, artist );
        musicInfoPainter.setFont( QFont( "Arial", image.height() / 15 ) );
        musicInfoPainter.drawText( image.width() / 100, image.height() - image.height() / 100, title );

        generateFromImage();
        emit imageWithMusicInfoReady( textureId );
    }

private slots:

    void slot_sourceCodeDownloaded( QNetworkReply *reply )
    {
        QString sourceCode = QString::fromUtf8( reply->readAll() );

        QString preImageCode = "&quot;,imgurl:&quot;", postImageCode = "&quot;,ow:&quot;";

        if( sourceCode.contains( preImageCode ) && sourceCode.contains( postImageCode ) )
        {
            sourceCode.remove( 0, sourceCode.indexOf( preImageCode ) + preImageCode.size() );
            QString externalUri = sourceCode.mid( 0, sourceCode.indexOf( postImageCode ) );
            imageManager->get( QNetworkRequest( QUrl( externalUri ) ) );
        }
        else emit notImage();
    }

    void slot_imageDownloaded( QNetworkReply *reply )
    {
        image = QImage::fromData( reply->readAll() );
        image = image.convertToFormat( QImage::Format_RGB32, Qt::ThresholdDither | Qt::AutoColor );
        image = image.scaled( 500, 500 );

        if( !image.isNull() )
        {
            generateFromImage();
            emit imageReady( textureId );
        }
        else emit notImage();
    }

signals:

    void notImage();
    void imageReady( GLuint );
    void imageWithMusicInfoReady( GLuint );
};

#endif // WEBIMAGE_H
