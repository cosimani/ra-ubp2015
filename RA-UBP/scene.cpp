#include "scene.h"

Scene::Scene( QWidget *parent ) : QGLWidget( parent ),
                                  videoCapture ( new cv::VideoCapture( 0 ) ),
                                  sceneTimer ( new QTimer ),
                                  musicActive( false ),

                                  textures( new QVector< Texture * > ),
                                  models( new QVector< Model * > ),
                                  videos( new QVector< Video * > ),
                                  webImageTextureId( 0 ),
                                  webMusicImageTextureId( 0 ),
                                  webMusicPlayer( new QMediaPlayer( this ) ),
                                  cameraParameters( new CameraParameters ),
                                  markerDetector( new MarkerDetector ),
                                  serial( new Serial ),
                                  valorServoHorizontal(90),
                                  camaraActual(0),

                                  detectMarkersOption( true ),
                                  rotateOption( false ),
                                  rotationVelocityOption( 5 ),
                                  descriptionOption( true ),
                                  scaleOption( 0 ),
                                  xTranslationOption( 0 ),
                                  yTranslationOption( 0 ),
                                  zTranslationOption( 0 ),
                                  xRotationOption( 0 ),
                                  yRotationOption( 0 ),
                                  zRotationOption( 0 ),
                                  detectFacesOption( false ),
                                  firstFaceOption( false ),
                                  linesWidthOption( 1 ),
                                  eraseMouthOption( false ),
                                  blurMouthOption( 32 ),
                                  showInBinary(false),
                                  seguirRostroOption(false)
{
    this->setMinimumSize( videoCapture->get( CV_CAP_PROP_FRAME_WIDTH ), videoCapture->get( CV_CAP_PROP_FRAME_HEIGHT ) );

    cameraParameters->readFromXMLFile( "../Files/CameraParameters.yml" );
    frontalFaceClassifier.load( "../Classifiers/FrontalFace.xml" );
    mouthClassifier.load( "../Classifiers/Mouth.xml" );
    sceneTimer->start( 10 );

    connect( webMusicPlayer, SIGNAL( bufferStatusChanged( int ) ), SLOT( slot_webMusicPlayerBufferChanged( int ) ) );
    connect( sceneTimer, SIGNAL( timeout() ), SLOT( slot_updateScene() ) );

    serial->enviar(90);

    bool isConectado = adminDB.conectar(QDir::currentPath() + "/../Databases/ra-ubp.sqlite");
    if (isConectado)
        qDebug() << "Se conecto con exito al archivo sqlite";


}

Scene::~Scene()  {
    delete serial;
}

void Scene::setWebImageTextureId( GLuint textureId )
{
    this->webImageTextureId = textureId;
}

void Scene::setWebMusicImageTextureId( GLuint textureId )
{
    this->webMusicImageTextureId = textureId;
}

void Scene::setWebMusicPlayerMedia( QString musicUri )
{
    if( musicUri == "" )
    {
        webMusicPlayer->stop();
        return;
    }
    webMusicPlayer->setMedia( QUrl( musicUri ) );
}

void Scene::cambiarCamara()  {

    if (!videoCapture->isOpened())
        return;

    QList<QCameraInfo> listaDeCamaras = QCameraInfo::availableCameras();

    delete videoCapture;
    camaraActual++;

    if(camaraActual >= listaDeCamaras.size())
        camaraActual = 0;

    videoCapture = new VideoCapture(camaraActual);


}

void Scene::loadTextures()
{
    QDir directory( "../Textures" );
    QStringList fileFilter;
    fileFilter << "*.jpg" << "*.png" << "*.bmp" << "*.gif";
    QStringList imageFiles = directory.entryList( fileFilter );

    for ( int i = 0; i < imageFiles.size(); i++ )
    {
        textures->append( new Texture( imageFiles.at( i ) ) );
        QString textureUri = "../Textures/" + imageFiles.at( i );

//        QImage imageTexturaComun;
//        if (!imageTexturaComun.load(textureUri))  {
//            qDebug() << "Recurso no disponible " + textureUri;
//            continue;
//        }

//        imageTexturaComun = QGLWidget::convertToGLFormat( imageTexturaComun );
////        texturaComun = imageTexturaComun.bits();
//        Mat textureMat (imageTexturaComun.height(), imageTexturaComun.width(), CV_8U, imageTexturaComun.bits());


        Mat textureMat = imread( textureUri.toStdString(), CV_LOAD_IMAGE_COLOR );
        flip( textureMat, textureMat, 0 );
        textures->last()->mat = textureMat;
        textures->last()->generateFromMat();
    }
}

void Scene::loadModels()
{
    QDir directory( "../Models" );
    QStringList fileFilter;
    fileFilter << "*.3ds";
    QStringList modelFiles = directory.entryList( fileFilter );

    for ( int i = 0 ; i < modelFiles.size() ; i++ )
        models->append( new Model( modelFiles.at( i ) ) );

    prepareModels();
}

void Scene::prepareModels()
{
    loadTexturesForModels();

    for ( int i = 0 ; i < models->size() ; i++)
    {
        if( !models->at( i ) ) return;

        models->at( i )->getFaces();
        Lib3dsVector *vertices = new Lib3dsVector[ models->at( i )->totalFaces * 3 ];
        Lib3dsVector *normals = new Lib3dsVector[ models->at( i )->totalFaces * 3 ];
        Lib3dsTexel *texCoords = new Lib3dsTexel[ models->at( i )->totalFaces * 3 ];
        Lib3dsMesh *mesh;

        unsigned int finishedFaces = 0;

        for( mesh = models->at(i)->model->meshes; mesh != NULL ; mesh = mesh->next )
        {
            lib3ds_mesh_calculate_normals( mesh, &normals[ finishedFaces * 3 ] );
            for( unsigned int currentFace = 0; currentFace < mesh->faces ; currentFace++ )
            {
                Lib3dsFace * face = &mesh->faceL[ currentFace ];
                for( unsigned int i = 0; i < 3; i++ )
                {
                    if( &mesh->texelL )
                        memcpy( &texCoords[ finishedFaces*3 + i ], mesh->texelL[ face->points[ i ] ], sizeof( Lib3dsTexel ) );
                    memcpy( &vertices[ finishedFaces * 3 + i ], mesh->pointL[ face->points[ i ] ].pos, sizeof( Lib3dsVector ) );
                }
                finishedFaces++;
            }
        }

        glGenBuffers( 1, &models->at(i)->vertexVBO );
        glBindBuffer( GL_ARRAY_BUFFER, models->at(i)->vertexVBO );
        glBufferData( GL_ARRAY_BUFFER, sizeof( Lib3dsVector ) * 3 * models->at(i)->totalFaces, vertices, GL_STATIC_DRAW );

        glGenBuffers( 1, &models->at(i)->normalVBO );
        glBindBuffer( GL_ARRAY_BUFFER, models->at(i)->normalVBO);
        glBufferData( GL_ARRAY_BUFFER, sizeof( Lib3dsVector ) * 3 * models->at(i)->totalFaces, normals, GL_STATIC_DRAW );

        glGenBuffers( 1, &models->at(i)->texCoordVBO );
        glBindBuffer( GL_ARRAY_BUFFER, models->at(i)->texCoordVBO);
        glBufferData( GL_ARRAY_BUFFER, sizeof( Lib3dsTexel ) * 3 * models->at(i)->totalFaces, texCoords, GL_STATIC_DRAW );

        delete vertices;
        delete normals;
        delete texCoords;

        lib3ds_file_free( models->at(i)->model );
        models->at(i)->model = NULL;
    }
}

void Scene::loadTexturesForModels()
{
    for ( int i = 0 ; i < models->size(); i++ )
    {
        QString modelTextureName = models->at( i )->name;
        modelTextureName.remove( ".3ds" );
        modelTextureName += ".jpg";

        for( int j = 0; j < textures->size(); j++ )
            if( textures->at( j )->name == modelTextureName )
                models->operator []( i )->textureId = textures->at( j )->id;
    }
}

void Scene::loadVideos()
{
    QDir directory( "../Videos" );
    QStringList fileFilter;
    fileFilter << "*.avi" << "*.aac" << "*.wmv" << "*.mpg" << "*.mpeg" << "*.mpeg1" << "*.mpeg2" << "*.mpeg4" << "*.mp4";
    QStringList videoFiles = directory.entryList( fileFilter );

    for ( int i = 0 ; i < videoFiles.size() ; i++ )
        videos->append( new Video( videoFiles.at( i ) ) );
}

void Scene::initializeGL()
{
    initializeGLFunctions();

    glClearColor( 0, 0, 0, 0 );
    glShadeModel( GL_SMOOTH );
    glEnable( GL_DEPTH_TEST );

    GLfloat lightAmbient[4]; lightAmbient[0] = 0.5f;  lightAmbient[1] = 0.5f;  lightAmbient[2] = 0.5f;  lightAmbient[3] = 1.0f;
    GLfloat lightDiffuse[4]; lightDiffuse[0] = 1.0f;  lightDiffuse[1] = 1.0f;  lightDiffuse[2] = 1.0f;  lightDiffuse[3] = 1.0f;
    GLfloat lightPosition[4];lightPosition[0]= 0.0f;  lightPosition[1]= 0.0f;  lightPosition[2]= 2.0f;  lightPosition[3]= 1.0f;

    glLightfv( GL_LIGHT1, GL_AMBIENT, lightAmbient );  glLightfv( GL_LIGHT1, GL_DIFFUSE, lightDiffuse );
    glLightfv( GL_LIGHT1, GL_POSITION,lightPosition ); glEnable( GL_LIGHT1 );

    textures->append( new Texture( "CameraTexture" ) );
    loadTextures();
    emit message( "Texturas cargadas" );
    loadModels();
    emit message( "Modelos cargadas" );
    loadVideos();
    emit message( "Videos cargados" );
}

void Scene::resizeGL( int width, int height )
{
    glViewport( 0, 0, width, height );
}

void Scene::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    glOrtho( 0, RESOLUTION_WIDTH, 0, RESOLUTION_HEIGHT, 1, 1000 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // Inicio: Gráfico de cámara

    glEnable( GL_TEXTURE_2D );
    glColor3f( 1, 1, 1 );
    glBindTexture( GL_TEXTURE_2D, textures->at( 0 )->id );
    glBegin( GL_QUADS );

        glTexCoord2f( 0, 0 ); glVertex3f( 0, RESOLUTION_HEIGHT, -999 );
        glTexCoord2f( 1, 0 ); glVertex3f( RESOLUTION_WIDTH, RESOLUTION_HEIGHT, -999 );
        glTexCoord2f( 1, 1 ); glVertex3f( RESOLUTION_WIDTH, 0, -999 );
        glTexCoord2f( 0, 1 ); glVertex3f( 0, 0, -999 );

    glEnd();
    glDisable( GL_TEXTURE_2D );

    // Fin: Gráfico de cámara

    glMatrixMode( GL_PROJECTION );
    double projectionMatrix[16];

    cv::Size2i sceneSize( RESOLUTION_WIDTH, RESOLUTION_HEIGHT );
    cv::Size2i openGlSize( RESOLUTION_WIDTH, RESOLUTION_HEIGHT );
    cameraParameters->glGetProjectionMatrix( sceneSize, openGlSize, projectionMatrix, 0.05, 10 );

    glLoadMatrixd( projectionMatrix );
    glMatrixMode( GL_MODELVIEW );
    double modelview_matrix[16];

    // Inicio: Gráfico en marcadores

    bool musicDetected = false;
    ThreeDWriter textoParaCredencial(80, "Arial", -0.04f, 0.045f, -0.001f, 0.3f, 0.02f, 0.0001f);
    QString comando_sql;
    QString textoParaMostrar;
    QVector<QStringList> vDatos;

    for( int i = 0 ; i < detectedMarkers.size() ; i++ )
    {
        detectedMarkers.operator []( i ).glGetModelViewMatrix( modelview_matrix );
        glLoadMatrixd( modelview_matrix );

        glTranslatef( xTranslationOption, yTranslationOption, zTranslationOption );
        if( rotateOption )
        {
            emit message( "Rotando" );
            zRotationOption += rotationVelocityOption;
        }
        glRotatef( xRotationOption, 1, 0, 0 );
        glRotatef( yRotationOption, 0, 1, 0 );
        glRotatef( zRotationOption, 0, 0, 1 );
                                                                                                                /*
||**************************************************************************************************************||
||                                                                                                              ||
||                                   MÉTODOS LISTOS PARA USAR EN MARCADORES:                                    ||
||                                                                                                              ||
||    DrawSheet - DrawBox - DrawCamera - DrawCameraBox - DrawModel - DrawVideo - DrawWebImage - ListenWebMusic  ||
||         Siempre agregar "+ scaleOption" al parámetro percentage para que la escala pueda ser modificada      ||
||                                                                                                              ||
|| *************************************************************************************************************||
||                                                                                                              ||
|| RECORDAR:   Ajustar la escala que se envía como parámetro ( percentage ).                                    ||
|| IMPORTANTE: Para que los modelos se vean bien, las imagenes y el sonido, agregar antes de usar:              ||
||                                                                                                              ||
|| DrawBox:        glRotatef( -90, 0, 1, 0 );                                                                   ||
|| DrawCamera:     glRotatef( 90, 0, 1, 0 );                                                                    ||
|| DrawCameraBox:  glRotatef( 90, 0, 1, 0 );                                                                    ||
|| DrawSheet:      glRotatef( 90, 0, 1, 0 );                                                                    ||
|| DrawWebImage:   glRotatef( 180, 0, 0, 1 ); glRotatef( -90, 0, 1, 0 );                                        ||
|| DrawVideo:      glRotatef( 90, 0, 1, 0 ); glRotatef( 180, 0, 0, 1 );                                         ||
|| ListenWebMusic: glRotatef( 180, 0, 0, 1 ); drawWebImage();                                                   ||
||                 musicDetected = true; webMusicPlayer->setVolume( detectedMarkers.at( i ).getArea() / 1000 ); ||
||                                                                                                              ||
|| House:          glRotatef( -90, 0, 1, 0 );                                                                   ||
|| IPhone:         glTranslatef( 0.032, 0, -0.025 ); glRotatef( 90, 1, 0, 0 );                                  ||
|| Man:            glTranslatef( 0, 0, -0.012 ); glRotatef( -90, 1, 0, 0 ); glRotatef( 90, 0, 0, 1 );           ||
|| Oil:            glRotatef( 90, 1, 0, 0 );                                                                    ||
|| Wagen:          glRotatef( 90, 1, 0, 0 ); glRotatef( 90, 0, 0, 1 );                                          ||
|| Woman:          glRotatef( 90, 1, 0, 0 ); glRotatef( 90, 0, 0, 1 );                                          ||
||                                                                                                              ||
|| *************************************************************************************************************||
                                                                                                                */

        switch( detectedMarkers.at( i ).id )
        {
            case 0: { glRotatef( 270, 1, 0, 0 );
                      glRotatef( 90, 0, 1, 0 );
                      drawCamera( 130 + scaleOption );
                      emit message( "Dibujando Cámara" );
                      break; }

            case 1: { glRotatef( 90, 1, 0, 0 );
                      glRotatef( 90, 0, 1, 0 );
                      drawCameraBox( 130 + scaleOption );
                      emit message( "Dibujando Cubo de la Cámara" );
                      break; }

            case 2: { glRotatef( 90+180, 1, 0, 0 );
                      glRotatef( 90, 0, 1, 0 );
                      drawWebImage( 150 + scaleOption );
                      break; }

            case 3: { glRotatef( -90, 1, 0, 0 );
                      glRotatef( 90, 0, 1, 0 );
                      drawWebMusicImage( 150 + scaleOption );
                      musicDetected = true;
                      int volume = detectedMarkers.at( i ).getArea() / 150;
                      if( volume > 100 ) volume = 100;
                      webMusicPlayer->setVolume( volume );
                      emit message( "Volumen audio: " +  QString::number( volume ) );
                      listenWebMusic(); break; }

            case 4: { glRotatef( 90, 1, 0, 0 );
                      glRotatef( 90, 0, 1, 0 );
                      drawBox( "Danger.jpg", 100 + scaleOption );
                      emit message( "Dibujando la Famosa Caja Danger" );
                      break; }

            case 5: { glRotatef( 180, 1, 0, 0 );
                      glRotatef( 90, 0, 1, 0 );
                      glTranslatef( -0.03, 0.05, 0);
                      drawModel( "IPhone.3ds", 70 + scaleOption );
                      emit message( "Dibujando Modelo 3d: IPhone" );
                      break;}

            case 6: { glTranslatef( 0, 0.02, 0 );
                      glRotatef( -90, 1, 0, 0 );
                      glRotatef( 90, 0, 1, 0 );
                      glRotatef( 90+180, 1, 0, 0 );
                      drawModel( "Man.3ds", 3 + scaleOption );
                      emit message( "Dibujando Modelo 3d: Man" );
                      break; }

            case 7: { glRotatef( 90, 0, 1, 0 );
                      glRotatef( 90, 1, 0, 0 );
                      glRotatef( 90, 0, 1, 0 );
                      drawModel( "Oil.3ds", 150 + scaleOption );
                      emit message( "Dibujando Modelo 3d: Oil" );
                      break; }

            case 8: { glRotatef( 270, 1, 0, 0 );
                      glRotatef( 90, 0, 1, 0 );
                      int volume = detectedMarkers.at( i ).getArea() / 150;
                      if( volume > 100 ) volume = 100;
                      emit message( "Volumen video: " +  QString::number( volume ) );
                      drawVideo( "trailer-RF7.mp4", volume, 200 + 2*scaleOption );
                      break; }

            case 9: { glRotatef( 90, 1, 0, 0 );
                       glRotatef( 90, 0, 1, 0 );
                       drawBox( "Ubp.png", 150 + scaleOption );
                       emit message( "Dibujando Cubo de la UBP" );
                       break; }

            case 10: {
                       if ( ! seguirRostroOption)
                           actualizaServoSiCorresponde(detectedMarkers.at( i ).getCenter().x, 50);
                       glBegin(GL_LINES);
                            glColor3f (1, 0, 0);
                            glVertex3f( 0, 0,  0);
                            glVertex3f( 0, 100,  0);
                            glColor3f (0, 0, 1);
                            glVertex3f( 0, 0,  0);
                            glVertex3f( -100, 0,  0);
                       glEnd();
                       emit message( "Control de Servo" );
                       break; }

            case 11: { glRotatef( 270, 1, 0, 0 );
                      glRotatef( 90, 0, 1, 0 );
                      int volume = detectedMarkers.at( i ).getArea() / 150;
                      if( volume > 100 ) volume = 100;
                      emit message( "Volumen video: " +  QString::number( volume ) );
                      drawVideo( "stic.mp4", volume, 200 + 2*scaleOption );
                      break; }

            case 12: { glRotatef( 270, 1, 0, 0 );
                      glRotatef( 90, 0, 1, 0 );
                      int volume = detectedMarkers.at( i ).getArea() / 150;
                      if( volume > 100 ) volume = 100;
                      emit message( "Volumen video: " +  QString::number( volume ) );
                      drawVideo( "trailer-relato.mp4", volume, 200 + 2*scaleOption );
                      break; }

            case 13:
                comando_sql = "SELECT sorteo FROM invitados WHERE apellido = 'House'";
                vDatos = adminDB.ejecutarComandoSelect(comando_sql);

                for (int i=0 ; i<vDatos.size() ; i++)  {
                    textoParaMostrar = vDatos.at(i).at(0);
                }

                glRotatef( 90, 1, 0, 0 );
                glRotatef( 270, 0, 1, 0 );
                glTranslatef( 0, 0, 0.005 );
                drawSheet( "drhouse.jpg", 130 + scaleOption );

                glLoadMatrixd( modelview_matrix );  // Cargamos la matriz inicial
                glRotatef( 90, 0, 0, 1 );
                glRotated(180, 0, 0, 5);
                textoParaCredencial.writeText(textoParaMostrar);

                break;

            case 14:
                comando_sql = "SELECT sorteo FROM invitados WHERE apellido = 'Messi'";
                vDatos = adminDB.ejecutarComandoSelect(comando_sql);

                for (int i=0 ; i<vDatos.size() ; i++)  {
                    textoParaMostrar = vDatos.at(i).at(0);
                }

                glRotatef( 90, 1, 0, 0 );
                glRotatef( 270, 0, 1, 0 );
                glTranslatef( 0, 0, 0.005 );
                drawSheet( "messi.png", 130 + scaleOption );

                glLoadMatrixd( modelview_matrix );  // Cargamos la matriz inicial
                glRotatef( 90, 0, 0, 1 );
                glRotated(180, 0, 0, 5);
                textoParaCredencial.writeText(textoParaMostrar);

                break;

            case 17: {
                    glRotatef( -90, 0, 0, 1 );
                    glRotatef( 90, 1, 0, 0 );
                    drawModel( "House.3ds", 40 + scaleOption );
                    emit message( "Dibujando Modelo 3d: Casa" );
                    break; }

            case 15: { glRotatef( 90, 1, 0, 0 );
                   glRotatef( 90, 0, 1, 0 );
                   drawBox( "caja.bmp", 100 + scaleOption );
                   emit message( "Dibujando Cubo de la UBP" );
                   break; }

            case 16: { glRotatef( 90, 1, 0, 0 );
                   glRotatef( 90, 0, 1, 0 );
                   drawBox( "caja.bmp", 100 + scaleOption );
                   emit message( "Dibujando Cubo de la UBP" );
                   break; }

            case 18: { glRotatef( 90, 1, 0, 0 );
                   glRotatef( 90, 0, 1, 0 );
                   drawBox( "caja.bmp", 100 + scaleOption );
                   emit message( "Dibujando Cubo de la UBP" );
                   break; }

            case 19: { glRotatef( 90, 1, 0, 0 );
                   glRotatef( 90, 0, 1, 0 );
                   drawBox( "caja.bmp", 100 + scaleOption );
                   emit message( "Dibujando Cubo de la UBP" );
                   break; }


            default:  { break; }
        }
    }

    if( musicDetected ) musicActive = true;
    else if( musicActive )
         {
            emit message( "<div style=\"color:red;\">Marcador no detectado, la música se pausará</div>" );
             webMusicPlayer->setVolume( webMusicPlayer->volume() - 1 );
             if( webMusicPlayer->volume() <= 0 )
             {
                 emit message( "Musica pausada" );
                 webMusicPlayer->pause();
                 musicActive = false;
             }
         }

    // La siguiente linea se ejecuta siempre. Habria que ingeniarsela de otra forma para bajar el volumen
    decreaseVideosVolume();

    // Fin: Gráfico en marcadores

    glFlush();
}


void Scene::actualizaServoSiCorresponde(int xActualMarcador, int franjaCentral)  {
//    qDebug() << "Actualizar servo" << xActualMarcador << yActualMarcador;

    // Si cae fuera de esto es porque no se detecto una cara en la imagen
    if (xActualMarcador > 0 && xActualMarcador < 640)  {

        // Dejamos una franja de 60 (30+30) pixeles para que la camara considere que estamos centrados
        if (xActualMarcador < 640/2 - franjaCentral)  {
            valorServoHorizontal+=1;  // Este es el avance, la velocidad de acomodarse hacia el destino
            valorServoHorizontal = qMin(valorServoHorizontal, 160);

            serial->enviar(valorServoHorizontal);
        }
        else  if (xActualMarcador > 640/2 + franjaCentral)  {
            valorServoHorizontal-=1;  // Este es el avance, la velocidad de acomodarse hacia el destino
            valorServoHorizontal = qMax(valorServoHorizontal, 20);
            serial->enviar(valorServoHorizontal);
        }

        if (valorServoHorizontal<0)
            valorServoHorizontal = 0;
        if (valorServoHorizontal>127)
            valorServoHorizontal = 127;

        serial->enviar(valorServoHorizontal);

    }
}



void Scene::process( Mat &frame )
{
    if( detectMarkersOption )
    {
//        Mat grayscaleMat; cvtColor( frame, grayscaleMat, CV_BGR2GRAY );
//        Mat binaryMat; threshold( grayscaleMat, binaryMat, 128, 255, cv::THRESH_BINARY );

        std::vector< Marker > detectedMarkersVector;
//        cameraParameters->resize( binaryMat.size() );
        cameraParameters->resize( frame.size() );
//        markerDetector->detect( binaryMat, detectedMarkersVector, *cameraParameters, 0.08f );
        markerDetector->detect( frame, detectedMarkersVector, *cameraParameters, 0.08f );
        detectedMarkers = QVector< Marker >::fromStdVector( detectedMarkersVector );

        if( descriptionOption )
            for( int i = 0; i < detectedMarkers.size(); i++ )
                detectedMarkers.at( i ).draw( frame, Scalar( 255, 0, 255 ), 1 );
    }
    else detectedMarkers.clear();

    if( detectFacesOption )
    {
        std::vector< Rect > detectedFaces;
        detectedFaces.clear();
        frontalFaceClassifier.detectMultiScale( frame, detectedFaces,
                                                1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size( 150, 150 ) );

        if( detectedFaces.size() > 0 )  {
            emit message( "<div style=\"color:green;\">Cantidad de caras detectadas: " +
                          QString::number( detectedFaces.size() ) + "</div>" );
            if (seguirRostroOption)
                actualizaServoSiCorresponde(detectedFaces.at( 0 ).x + detectedFaces.at( 0 ).width/2);
        }
        else
            emit message( "Cantidad de caras detectadas: " + QString::number( detectedFaces.size() ) );

        if( firstFaceOption )
        {
            if( detectedFaces.size() > 0 )
            {
                emit message( "Recortando una cara" );
                detectedFaces.at( 0 ).width = detectedFaces.at( 0 ).width - detectedFaces.at( 0 ).width % 4;
                Mat faceMat = Mat( frame, detectedFaces.at( 0 ) );
                frame = faceMat.clone();
            }
        }

        else if( !eraseMouthOption )
            for( unsigned int i = 0; i < detectedFaces.size(); i++ )
                circle( frame, Point( detectedFaces.at( i ).x + ( detectedFaces.at( i ).width / 2 ),
                                      detectedFaces.at( i ).y + ( detectedFaces.at( i ).height / 2 ) ),
                        ( detectedFaces.at( i ).width / 2 ), Scalar( 255, 0, 0 ), linesWidthOption );

        if( eraseMouthOption )
        for( unsigned int i = 0; i < detectedFaces.size(); i ++ )
        {
            int xMouthZone = detectedFaces.at( i ).x + detectedFaces.at( i ).width * 0.15;
            int yMouthZone = detectedFaces.at( i ).y + ( detectedFaces.at( i ).height / 2 ) + ( detectedFaces.at( i ).height * 0.15 );
            int wMouthZone = detectedFaces.at( i ).width * 0.70;
            int hMouthZone = detectedFaces.at( i ).height / 2;
            Rect mouthZoneRect( xMouthZone, yMouthZone, wMouthZone, hMouthZone );

            if( xMouthZone + wMouthZone > frame.cols ) return;
            if( yMouthZone + hMouthZone > frame.rows ) return;

            Mat mouthZoneMat( frame, mouthZoneRect );
            std::vector<Rect> mouthsDetected;
            mouthsDetected.clear();
            mouthClassifier.detectMultiScale( mouthZoneMat, mouthsDetected,
                                               1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size( 50, 50 ) );

            if( mouthsDetected.size() > 0 )
            {
                emit message( "Borrando la boca" );

                Rect correctedMouthRect( mouthsDetected.at( 0 ).x - mouthsDetected.at( 0 ).width * 0.15,
                                         mouthsDetected.at( 0 ).y - mouthsDetected.at( 0 ).height * 0.28,
                                         mouthsDetected.at( 0 ).width * 1.3, mouthsDetected.at( 0 ).height * 1.4 );

                Rect correctedMouthRectToGlobal( mouthZoneRect.x + correctedMouthRect.x,
                                                 mouthZoneRect.y + correctedMouthRect.y,
                                                 correctedMouthRect.width, correctedMouthRect.height );

                Mat mouthMat( frame, correctedMouthRectToGlobal );
                Mat mouthMatCopy = mouthMat.clone();
                blur( mouthMatCopy, mouthMatCopy, Size( blurMouthOption, blurMouthOption ) );
                toFourChannels( mouthMatCopy, mouthMatCopy );
                blurBorders( mouthMatCopy );
                overlayImage( frame, mouthMatCopy, frame,
                              Point( correctedMouthRectToGlobal.x, correctedMouthRectToGlobal.y ) );
            }
        }
    }
}

void Scene::blurBorders( Mat &foreground )
{
    int blur = 16;
    for( int i = 0; i < foreground.rows; i++ )
    for( int j = 0; j < foreground.cols; j++ ) {

    // Esquinas
    if( i < blur && j < blur )
    {   if( i < j )
        { Vec4b& v = foreground.at<Vec4b>( i, j ); v[3] = abs( ( i * blur ) - 1 ); }
        else
        { Vec4b& v = foreground.at<Vec4b>( i, j ); v[3] = abs( ( j * blur ) - 1 ); } }
    if( i > foreground.rows - blur && j > foreground.cols - blur )
    {   if( foreground.rows - i > foreground.cols - j )
        { Vec4b& v = foreground.at<Vec4b>( i, j ); v[3] = abs( ( ( foreground.cols - j ) * blur ) - 1 ); }
        else
        { Vec4b& v = foreground.at<Vec4b>( i, j ); v[3] = abs( ( ( foreground.rows - i ) * blur ) - 1 ); } }
    if( i > foreground.rows - blur && j < blur )
    {   if( foreground.rows - i < j )
        { Vec4b& v = foreground.at<Vec4b>( i, j ); v[3] = abs( ( ( foreground.rows - i ) * blur ) - 1 ); }
        else
        { Vec4b& v = foreground.at<Vec4b>( i, j ); v[3] = abs( ( j * blur ) - 1 ); } }
    if( i < blur && j > foreground.cols - blur )
    {   if( ( foreground.cols - j ) < i )
        { Vec4b& v = foreground.at<Vec4b>( i, j ); v[3] = abs( ( ( foreground.cols - j ) * blur ) - 1 ); }
        else
        { Vec4b& v = foreground.at<Vec4b>( i, j ); v[3] = abs( ( i * blur ) - 1 ); } }

    // Bordes
    if( i < blur && j > ( blur - 1 ) && j < foreground.cols - ( blur - 1 ) )
    { Vec4b& v = foreground.at<Vec4b>( i, j ); v[3] = abs( ( i * blur ) - 1 ); }
    if( j < blur && i > ( blur - 1 ) && i < foreground.rows - ( blur - 1 ) )
    { Vec4b& v = foreground.at<Vec4b>( i, j ); v[3] = abs( ( j * blur ) - 1 ); }
    if( i > foreground.rows - blur && j > ( blur - 1 ) && j < foreground.cols - ( blur - 1 ) )
    { Vec4b& v = foreground.at<Vec4b>( i, j ); v[3] = abs( ( ( foreground.rows - i ) * blur ) - 1 ); }
    if( j > foreground.cols - blur && i > ( blur - 1 ) && i < foreground.rows - ( blur - 1 ) )
    { Vec4b& v = foreground.at<Vec4b>( i, j ); v[3] = abs( ( ( foreground.cols - j ) * blur ) - 1 ); }
    if( i > ( blur - 1 ) && j > ( blur - 1 ) && i < foreground.rows - ( blur - 1 )
            && j < foreground.cols - ( blur - 1 ) )
    { Vec4b& v = foreground.at<Vec4b>( i, j ); v[3] = 255; } }
}

void Scene::overlayImage( const Mat &background, const Mat &foreground, Mat &output, Point2i location )
{
    background.copyTo(output);
    for( int y = max( location.y , 0 ); y < background.rows; ++y )
    {
        int fY = y - location.y;
        if( fY >= foreground.rows ) break;
        for( int x = max( location.x, 0 ); x < background.cols; ++x )
        {
            int fX = x - location.x;
            if( fX >= foreground.cols ) break;
            double opacity = ( ( double )foreground.data[fY * foreground.step +
                    fX * foreground.channels() + 3] ) / ( float )255;
            for( int c = 0; opacity > 0 && c < output.channels(); ++c )
            {
                unsigned char foregroundPx = foreground.data[fY * foreground.step + fX * foreground.channels() + c];
                unsigned char backgroundPx = background.data[y * background.step + x * background.channels() + c];
                output.data[ y * output.step + output.channels() * x + c ] =
                        backgroundPx * ( ( float )1 - opacity ) + foregroundPx * opacity;
            }
        }
    }
}

void Scene::toFourChannels( Mat threeChannels, Mat &fourChannels )
{
    fourChannels = Mat( threeChannels.rows, threeChannels.cols, CV_8UC4 );

    for( int i = 0; i < fourChannels.rows; i++ )
        for( int j = 0; j < fourChannels.cols; j++ )
        {
            Vec4b& fourChannelsValues = fourChannels.at<Vec4b>( i, j );
            Vec3b& threeChannelsValues = threeChannels.at<Vec3b>( i, j );
            fourChannelsValues[0] = threeChannelsValues[0];
            fourChannelsValues[1] = threeChannelsValues[1];
            fourChannelsValues[2] = threeChannelsValues[2];
            fourChannelsValues[3] = 255;
        }
}

void Scene::drawCamera( int percentage )
{
    drawSheet( "CameraTexture", percentage );
}

void Scene::drawCameraBox( int percentage )
{
    drawBox( "CameraTexture", percentage );
}

void Scene::drawSheet( QString textureName, int percentage )
{
    for( int i = 0; i < textures->size(); i++ )
        if( textures->at( i )->name == textureName )
        {
            float sideLength = percentage / ( float )2300;
            glEnable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, textures->at( i )->id );
            glColor3f( 1, 1, 1 );
            glRotated( 90, 1, 0, 0 );
            glTranslatef( 0, 0, sideLength );
            glBegin( GL_QUADS );

                glNormal3f( 0.0f, 0.0f,-1.0f);
                glTexCoord2f( 1.0f, 0.0f ); glVertex3f(-sideLength, -sideLength, -sideLength );
                glTexCoord2f( 1.0f, 1.0f ); glVertex3f(-sideLength,  sideLength, -sideLength );
                glTexCoord2f( 0.0f, 1.0f ); glVertex3f( sideLength,  sideLength, -sideLength );
                glTexCoord2f( 0.0f, 0.0f ); glVertex3f( sideLength, -sideLength, -sideLength );

            glEnd();
            glDisable( GL_TEXTURE_2D);
        }
}

void Scene::drawBox( QString textureName, int percentage )
{
    for( int i = 0; i < textures->size(); i++ )
        if( textures->at( i )->name == textureName )
        {
            float sideLength = percentage / ( float )2300;

            glEnable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, textures->at( i )->id );
            glColor3f( 1, 1, 1 );
            glRotated( 90, 1, 0, 0 );
            glTranslatef( 0, 0, -sideLength );
            glEnable( GL_LIGHTING );
            glBegin( GL_QUADS );

                glNormal3f( 0.0f, 0.0f, 1.0f ); // Frontal
                glTexCoord2f( 0.0f, 0.0f ); glVertex3f(-sideLength, -sideLength,  sideLength );
                glTexCoord2f( 1.0f, 0.0f ); glVertex3f( sideLength, -sideLength,  sideLength );
                glTexCoord2f( 1.0f, 1.0f ); glVertex3f( sideLength,  sideLength,  sideLength );
                glTexCoord2f( 0.0f, 1.0f ); glVertex3f(-sideLength,  sideLength,  sideLength );

                glNormal3f( 0.0f, 0.0f,-1.0f ); // Anterior
                glTexCoord2f( 1.0f, 0.0f ); glVertex3f(-sideLength, -sideLength, -sideLength );
                glTexCoord2f( 1.0f, 1.0f ); glVertex3f(-sideLength,  sideLength, -sideLength );
                glTexCoord2f( 0.0f, 1.0f ); glVertex3f( sideLength,  sideLength, -sideLength );
                glTexCoord2f( 0.0f, 0.0f ); glVertex3f( sideLength, -sideLength, -sideLength );

                glNormal3f( 0.0f, 1.0f, 0.0f ); // Superior
                glTexCoord2f( 0.0f, 1.0f ); glVertex3f(-sideLength,  sideLength, -sideLength );
                glTexCoord2f( 0.0f, 0.0f ); glVertex3f(-sideLength,  sideLength,  sideLength );
                glTexCoord2f( 1.0f, 0.0f ); glVertex3f( sideLength,  sideLength,  sideLength );
                glTexCoord2f( 1.0f, 1.0f ); glVertex3f( sideLength,  sideLength, -sideLength );

                glNormal3f( 0.0f,-1.0f, 0.0f ); // Inferior
                glTexCoord2f( 1.0f, 1.0f ); glVertex3f(-sideLength, -sideLength, -sideLength );
                glTexCoord2f( 0.0f, 1.0f ); glVertex3f( sideLength, -sideLength, -sideLength );
                glTexCoord2f( 0.0f, 0.0f ); glVertex3f( sideLength, -sideLength,  sideLength );
                glTexCoord2f( 1.0f, 0.0f ); glVertex3f(-sideLength, -sideLength,  sideLength );

                glNormal3f( 1.0f, 0.0f, 0.0f ); // Derecha
                glTexCoord2f( 1.0f, 0.0f ); glVertex3f( sideLength, -sideLength, -sideLength );
                glTexCoord2f( 1.0f, 1.0f ); glVertex3f( sideLength,  sideLength, -sideLength );
                glTexCoord2f( 0.0f, 1.0f ); glVertex3f( sideLength,  sideLength,  sideLength );
                glTexCoord2f( 0.0f, 0.0f ); glVertex3f( sideLength, -sideLength,  sideLength );

                glNormal3f( -1.0f, 0.0f, 0.0f ); // Izquierda
                glTexCoord2f( 0.0f, 0.0f ); glVertex3f(-sideLength, -sideLength, -sideLength );
                glTexCoord2f( 1.0f, 0.0f ); glVertex3f(-sideLength, -sideLength,  sideLength );
                glTexCoord2f( 1.0f, 1.0f ); glVertex3f(-sideLength,  sideLength,  sideLength );
                glTexCoord2f( 0.0f, 1.0f ); glVertex3f(-sideLength,  sideLength, -sideLength );

            glEnd();
            glDisable( GL_LIGHTING );
            glDisable( GL_TEXTURE_2D );
        }
}

void Scene::drawModel( QString modelName, int percentage )
{
    float scale = percentage / ( float )1000;
    for ( int i = 0 ; i < models->size(); i++ )
        if ( models->at( i )->name == modelName )
        {
            if( !models->at( i )->totalFaces ) return;

            glEnable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, models->at( i )->textureId );
            glScalef( scale, scale, -scale );
            glEnableClientState( GL_VERTEX_ARRAY );
            glEnableClientState( GL_NORMAL_ARRAY );
            glEnableClientState( GL_TEXTURE_COORD_ARRAY );

                glBindBuffer( GL_ARRAY_BUFFER, models->at( i )->normalVBO );
                glNormalPointer( GL_FLOAT, 0, NULL );
                glBindBuffer( GL_ARRAY_BUFFER, models->at( i )->texCoordVBO );
                glTexCoordPointer( 2, GL_FLOAT, 0, NULL );
                glBindBuffer( GL_ARRAY_BUFFER, models->at( i )->vertexVBO );
                glVertexPointer( 3, GL_FLOAT, 0, NULL );
                glDrawArrays( GL_TRIANGLES, 0, models->at( i )->totalFaces * 3 );

            glDisableClientState( GL_VERTEX_ARRAY );
            glDisableClientState( GL_NORMAL_ARRAY );
            glDisableClientState( GL_TEXTURE_COORD_ARRAY );

            glDisable( GL_TEXTURE_2D );
        }
}

void Scene::drawVideo( QString videoName, int volume, int percentage )
{
    for ( int i = 0 ; i < videos->size(); i++ )
        if ( videos->at( i )->name == videoName )
        {
            videos->at( i )->player->play();
            videos->at( i )->player->setVolume( volume );

            float sideLength = percentage / ( float )2300;

            glEnable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, videos->at( i )->grabber->textureId );
            glRotated( 90, 1, 0, 0 );
            glTranslatef( 0, 0, sideLength );
            glColor3f( 1, 1, 1 );
            glBegin( GL_QUADS );

                glNormal3f( 0.0f, 0.0f,-1.0f);
                glTexCoord2f( 1.0f, 0.0f ); glVertex3f(-sideLength*( 16 / ( float )9 ), -sideLength, -sideLength );
                glTexCoord2f( 1.0f, 1.0f ); glVertex3f(-sideLength*( 16 / ( float )9 ),  sideLength, -sideLength );
                glTexCoord2f( 0.0f, 1.0f ); glVertex3f( sideLength*( 16 / ( float )9 ),  sideLength, -sideLength );
                glTexCoord2f( 0.0f, 0.0f ); glVertex3f( sideLength*( 16 / ( float )9 ), -sideLength, -sideLength );

            glEnd();
            glDisable( GL_TEXTURE_2D);
        }
}

void Scene::decreaseVideosVolume()
{
    for ( int i = 0 ; i < videos->size(); i++ )

        if ( videos->at( i )->player->state() == QMediaPlayer::PlayingState )  {
//            emit message( "<div style=\"color:red;\">Marcador no detectado, el video se pausará</div>" );
            videos->at( i )->player->setVolume( videos->at( i )->player->volume() - 1 );

            if( videos->at( i )->player->volume() <= 0 )
            {
//                emit message( "Video pausado" );
                videos->at( i )->player->pause();
            }
        }
}


void Scene::decreaseVideoVolume(QString videoName)
{
    for ( int i = 0 ; i < videos->size(); i++ )
        if ( videos->at( i )->name == videoName )
        {
            emit message( "<div style=\"color:red;\">Marcador no detectado, el video se pausará</div>" );
            videos->at( i )->player->setVolume( videos->at( i )->player->volume() - 1 );

            qDebug() << videoName << videos->at( i )->player->volume();
            if( videos->at( i )->player->volume() <= 0 )
            {
                emit message( "Video pausado" );
                videos->at( i )->player->pause();
//                videoActive = false;
            }
        }
}

void Scene::drawWebImage( int percentage )
{
    if( webImageTextureId == 0 ) return;
    emit message( "Dibujando Imagen de la Web" );

    float sideLength = percentage / ( float )2300;

    // Grafico
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, webImageTextureId );
    glColor3f( 1, 1, 1 );
    glRotated( 90, 1, 0, 0 );
    glTranslatef( 0, 0, sideLength );
    glBegin( GL_QUADS );

            glNormal3f( 0.0f, 0.0f,-1.0f);
            glTexCoord2f( 1.0f, 0.0f ); glVertex3f(-sideLength, -sideLength, -sideLength );
            glTexCoord2f( 1.0f, 1.0f ); glVertex3f(-sideLength,  sideLength, -sideLength );
            glTexCoord2f( 0.0f, 1.0f ); glVertex3f( sideLength,  sideLength, -sideLength );
            glTexCoord2f( 0.0f, 0.0f ); glVertex3f( sideLength, -sideLength, -sideLength );

    glEnd();
    glDisable( GL_TEXTURE_2D);
}

void Scene::drawWebMusicImage( int percentage )
{
    if( webMusicImageTextureId == 0 ) return;

    float sideLength = percentage / ( float )2300;

    // Grafico
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, webMusicImageTextureId );
    glColor3f( 1, 1, 1 );
    glRotated( 90, 1, 0, 0 );
    glTranslatef( 0, 0, sideLength );
    glBegin( GL_QUADS );

            glNormal3f( 0.0f, 0.0f,-1.0f);
            glTexCoord2f( 1.0f, 0.0f ); glVertex3f(-sideLength, -sideLength, -sideLength );
            glTexCoord2f( 1.0f, 1.0f ); glVertex3f(-sideLength,  sideLength, -sideLength );
            glTexCoord2f( 0.0f, 1.0f ); glVertex3f( sideLength,  sideLength, -sideLength );
            glTexCoord2f( 0.0f, 0.0f ); glVertex3f( sideLength, -sideLength, -sideLength );

    glEnd();
    glDisable( GL_TEXTURE_2D);
}

void Scene::listenWebMusic()
{
    if( webMusicPlayer->state() == QMediaPlayer::PlayingState ) return;
    if( webMusicPlayer->mediaStatus() == QMediaPlayer::BufferingMedia ) return;
    webMusicPlayer->play();
}

void Scene::slot_updateScene()
{
    videoCapture->operator >>( textures->operator []( 0 )->mat );

    if ( showInBinary )  {
        Mat frameCorners = textures->operator []( 0 )->mat.clone();
        Mat grayscaleMat;
        cvtColor( frameCorners, grayscaleMat, CV_BGR2GRAY );
        Mat binaryMat;
        threshold( grayscaleMat, binaryMat, 128, 255, cv::THRESH_BINARY );
        Mat enColor;
        cvtColor( binaryMat, enColor, CV_GRAY2RGB );
        textures->operator [](0)->mat = enColor.clone();
    }

    process( textures->operator []( 0 )->mat );
    textures->operator []( 0 )->generateFromMat();
    this->updateGL();
}

void Scene::slot_webMusicPlayerBufferChanged( int percentageLoaded )
{
    emit message( "Cargando Audio: " + QString::number( percentageLoaded ) + "%" );
}
