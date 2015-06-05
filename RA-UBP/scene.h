#ifndef SCENE_H
#define SCENE_H

#define RESOLUTION_WIDTH  640
#define RESOLUTION_HEIGHT 480

#include <QDir>
#include <QFile>
#include <QTimer>
#include <QVector>
#include <QGLWidget>
#include <QGLFunctions>
#include <QCameraInfo>

#include "serial.h"
#include "adminDB.h"
#include "threedwriter.h"

#include "texture.h"
#include "model.h"
#include "video.h"

#include <aruco/aruco.h>
#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace aruco;

class Scene : public QGLWidget, protected QGLFunctions
{
    Q_OBJECT

private:

    VideoCapture *videoCapture;
    QTimer *sceneTimer;
    bool musicActive;

    QVector< Texture * > *textures;
    QVector< Model * > *models;
    QVector< Video * > *videos;
    GLuint webImageTextureId;
    GLuint webMusicImageTextureId;
    QMediaPlayer *webMusicPlayer;

    CameraParameters *cameraParameters;
    MarkerDetector *markerDetector;
    QVector< Marker > detectedMarkers;

    CascadeClassifier frontalFaceClassifier;
    CascadeClassifier mouthClassifier;

    Serial * serial;
    int valorServoHorizontal;
    int camaraActual;

    AdminDB adminDB;


    void actualizaServoSiCorresponde(int xActualMarcador, int franjaCentral = 30);


    void loadTextures();
    void loadModels();
    void prepareModels();
    void loadTexturesForModels();
    void loadVideos();

    void process( Mat &frame );

    void blurBorders( Mat &foreground );
    void overlayImage( const Mat &background, const Mat &foreground, Mat &output, Point2i location );
    void toFourChannels( Mat threeChannels, Mat &fourChannels );

    void drawCamera( int percentage = 100 );
    void drawCameraBox( int percentage = 100 );
    void drawSheet( QString textureName, int percentage = 100 );
    void drawBox( QString textureName, int percentage = 100 );
    void drawModel( QString modelName, int percentage = 100 );
    void drawVideo( QString videoName, int volume = 100, int percentage = 100 );
    void decreaseVideosVolume();
    void decreaseVideoVolume( QString videoName );
    void drawWebImage( int percentage = 100 );
    void drawWebMusicImage( int percentage = 100 );
    void listenWebMusic();

public:

    Scene( QWidget *parent = 0 );
    virtual ~Scene();

    void setWebImageTextureId( GLuint textureId );
    void setWebMusicImageTextureId( GLuint textureId );
    void setWebMusicPlayerMedia( QString musicUri );

    void cambiarCamara();

    bool detectMarkersOption;
    bool rotateOption;
    int rotationVelocityOption;
    bool descriptionOption;
    int scaleOption;
    double xTranslationOption;
    double yTranslationOption;
    double zTranslationOption;
    int xRotationOption;
    int yRotationOption;
    int zRotationOption;
    bool detectFacesOption;
    bool firstFaceOption;
    int linesWidthOption;
    bool eraseMouthOption;
    int blurMouthOption;
    bool showInBinary;
    bool seguirRostroOption;


protected:

    void initializeGL();
    void resizeGL( int width, int height );
    void paintGL();

private slots:

    void slot_updateScene();
    void slot_webMusicPlayerBufferChanged( int percentageLoaded );

signals:

    void message( QString text );
};

#endif // SCENE_H
