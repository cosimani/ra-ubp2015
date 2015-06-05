#ifndef LOGO_H
#define LOGO_H

#include <QWidget>
#include <QImage>
#include <QPainter>

class Logo : public QWidget
{
private:

    QImage image;

public:

    Logo( QWidget *parent = 0 ) : QWidget( parent )
    {
        image.load( "../Textures/AppLogo.png" );
        if( !image.isNull() ) this->setFixedSize( image.width(), image.height() );
        this->repaint();
    }

protected:

    void paintEvent( QPaintEvent * )
    {
        QPainter painter( this );
        if( !image.isNull() )
            painter.drawImage( 0, 0, image );
    }
};

#endif // LOGO_H
