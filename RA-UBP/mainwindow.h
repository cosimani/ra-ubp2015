#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCompleter>
#include <QKeyEvent>
#include "webimage.h"
#include "webmusic.h"

namespace Ui { class MainWindow; }

class MainWindow : public QWidget
{
    Q_OBJECT

private:    
    Ui::MainWindow *ui;

    QNetworkAccessManager *suggestionsManager;

    void enableImagesSearch();
    void enableMusicSearch();

public:

    explicit MainWindow( QWidget *parent = 0 );
    ~MainWindow();


private slots:

    void slot_searchTextChanged( QString search );
    void slot_suggestionsDownloaded( QNetworkReply *reply );
    void slot_writeMessage( QString text );

    void slot_searchImagesButtonClicked();
    void slot_webImageNotImage();
    void slot_webImageReady( GLuint textureId );

    void slot_searchMusicButtonClicked();
    void slot_webMusicNotMusic();
    void slot_webMusicReady( QString uri );
    void slot_webMusicAssociatedImageReady( GLuint textureId );

    void slot_detectMarkersCheckBoxValueChanged( bool checked );
    void slot_rotateCheckBoxValueChanged( bool checked );
    void slot_rotationVelocitySliderValueChanged( int value );
    void slot_descriptionCheckBoxValueChanged( bool checked );
    void slot_scaleSpinBoxValueChanged( int value );

    void slot_xTranslationSpinBoxValueChanged( double value );
    void slot_yTranslationSpinBoxValueChanged( double value );
    void slot_zTranslationSpinBoxValueChanged( double value );

    void slot_xRotationSpinBoxValueChanged( int value );
    void slot_yRotationSpinBoxValueChanged( int value );
    void slot_zRotationSpinBoxValueChanged( int value );

    void slot_detectFacesCheckBoxValueChanged( bool checked );
    void slot_firstFaceCheckBoxValueChanged( bool checked );
    void slot_linesWidthSliderValueChanged( int value );
    void slot_eraseMouthCheckBoxValueChanged( bool checked );
    void slot_blurSliderValueChanged( int value );
    void slot_seguirRostroCheckBoxValueChanged( bool checked );

protected:
    void keyPressEvent(QKeyEvent *e);

};

#endif // MAINWINDOW_H
