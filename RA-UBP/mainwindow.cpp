#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow( QWidget *parent ) : QWidget( parent ),
                                            ui( new Ui::MainWindow ),
                                            suggestionsManager( new QNetworkAccessManager( this ) )
{
    ui->setupUi( this );
    ui->mainWindowLayout->setColumnStretch( 0, 1 );

    this->setFont( QFont( "Tahoma", 8 ) );

    connect( ui->searchImagesLineEdit, SIGNAL( textChanged( QString ) ), SLOT( slot_searchTextChanged( QString ) ) );
    connect( ui->searchMusicLineEdit, SIGNAL( textChanged( QString ) ), SLOT( slot_searchTextChanged( QString ) ) );
    connect( suggestionsManager, SIGNAL( finished( QNetworkReply* ) ), SLOT( slot_suggestionsDownloaded( QNetworkReply* ) ) );
    connect( ui->scene, SIGNAL( message( QString ) ), SLOT( slot_writeMessage( QString ) ) );

    connect( ui->searchImagesLineEdit, SIGNAL( returnPressed() ), SLOT( slot_searchImagesButtonClicked() ) );
    connect( ui->searchImagesButton, SIGNAL( clicked() ), SLOT( slot_searchImagesButtonClicked() ) );

    connect( ui->searchMusicLineEdit, SIGNAL( returnPressed() ), SLOT( slot_searchMusicButtonClicked() ) );
    connect( ui->searchMusicButton, SIGNAL( clicked() ), SLOT( slot_searchMusicButtonClicked() ) );

    connect( ui->detectMarkersCheckBox, SIGNAL( toggled( bool ) ), SLOT( slot_detectMarkersCheckBoxValueChanged( bool ) ) );
    connect( ui->rotateCheckBox, SIGNAL( toggled( bool ) ), SLOT( slot_rotateCheckBoxValueChanged( bool ) ) );
    connect( ui->rotationVelocitySlider, SIGNAL( valueChanged( int ) ), SLOT( slot_rotationVelocitySliderValueChanged( int ) ) );
    connect( ui->descriptionCheckBox, SIGNAL( toggled( bool ) ), SLOT( slot_descriptionCheckBoxValueChanged( bool ) ) );
    connect( ui->scaleSpinBox, SIGNAL( valueChanged( int ) ), SLOT( slot_scaleSpinBoxValueChanged( int ) ) );

    connect( ui->xTranslationSpinBox, SIGNAL( valueChanged( double ) ), SLOT( slot_xTranslationSpinBoxValueChanged( double ) ) );
    connect( ui->yTranslationSpinBox, SIGNAL( valueChanged( double ) ), SLOT( slot_yTranslationSpinBoxValueChanged( double ) ) );
    connect( ui->zTranslationSpinBox, SIGNAL( valueChanged( double ) ), SLOT( slot_zTranslationSpinBoxValueChanged( double ) ) );

    connect( ui->xRotationSpinBox, SIGNAL( valueChanged( int ) ), SLOT( slot_xRotationSpinBoxValueChanged( int ) ) );
    connect( ui->yRotationSpinBox, SIGNAL( valueChanged( int ) ), SLOT( slot_yRotationSpinBoxValueChanged( int ) ) );
    connect( ui->zRotationSpinBox, SIGNAL( valueChanged( int ) ), SLOT( slot_zRotationSpinBoxValueChanged( int ) ) );

    connect( ui->detectFacesCheckBox, SIGNAL( toggled( bool ) ), SLOT( slot_detectFacesCheckBoxValueChanged( bool ) ) );
    connect( ui->firstFaceCheckBox, SIGNAL( toggled( bool ) ), SLOT( slot_firstFaceCheckBoxValueChanged( bool ) ) );
    connect( ui->linesWidthSlider, SIGNAL( valueChanged( int ) ), SLOT( slot_linesWidthSliderValueChanged( int ) ) );
    connect( ui->eraseMouthCheckBox, SIGNAL( toggled( bool ) ), SLOT( slot_eraseMouthCheckBoxValueChanged( bool ) ) );
    connect( ui->blurMouthSlider, SIGNAL( valueChanged( int ) ), SLOT( slot_blurSliderValueChanged( int ) ) );
    connect( ui->seguirRostroCheckBox, SIGNAL( toggled( bool ) ), SLOT( slot_seguirRostroCheckBoxValueChanged( bool ) ) );
}

void MainWindow::enableImagesSearch()
{
    ui->searchImagesButton->setText( "Buscar" );
    ui->searchImagesButton->setEnabled( true );
    ui->searchImagesLineEdit->setEnabled( true );
    ui->searchImagesLineEdit->setFocus();
    ui->searchImagesLineEdit->selectAll();
}

void MainWindow::enableMusicSearch()
{
    ui->searchMusicButton->setText( "Buscar" );
    ui->searchMusicButton->setEnabled( true );
    ui->searchMusicLineEdit->setEnabled( true );
    ui->searchMusicLineEdit->setFocus();
    ui->searchMusicLineEdit->selectAll();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slot_searchTextChanged( QString search )
{
    suggestionsManager->get( QNetworkRequest( QUrl( "https://www.google.com/complete/search?output=toolbar&q=" + search ) ) );
}

void MainWindow::slot_suggestionsDownloaded( QNetworkReply *reply )
{
    QString xmlCode = QString::fromUtf8( reply->readAll() );

    QString preSuggestionCode = "<suggestion data=\"";
    QString postSuggestionCode = "\"/>";

    QStringList *suggestions = new QStringList;

    while( xmlCode.contains( preSuggestionCode ) )
    {
        xmlCode.remove( 0, xmlCode.indexOf( preSuggestionCode ) + preSuggestionCode.size() );
        suggestions->append( xmlCode.mid( 0, xmlCode.indexOf( postSuggestionCode ) ) );
    }

    QCompleter *completer = new QCompleter( *suggestions );
    completer->setCaseSensitivity( Qt::CaseInsensitive );
    completer->setCompletionMode( QCompleter::InlineCompletion );

    ui->searchImagesLineEdit->setCompleter( completer );
    ui->searchMusicLineEdit->setCompleter( completer );
}

void MainWindow::slot_writeMessage( QString text )
{
    ui->messagesTextEdit->append( text );
}

void MainWindow::slot_searchImagesButtonClicked()
{
    ui->searchImagesLineEdit->setText( ui->searchImagesLineEdit->text().simplified() );
    if( ui->searchImagesLineEdit->text() == "" )
    {
        ui->scene->setWebImageTextureId( 0 );
        return;
    }

    ui->searchImagesLineEdit->setEnabled( false );
    ui->searchImagesButton->setEnabled( false );
    ui->searchImagesButton->setText( "Buscando" );


    WebImage *webImage = new WebImage( this );

    if( ui->allImagesButton->isChecked() )
    {
        ui->messagesTextEdit->append( "Buscando imagenes: " + ui->searchImagesLineEdit->text() + "..." );
        webImage->searchForAllImages( ui->searchImagesLineEdit->text() );
    }
    if( ui->streetViewImagesButton->isChecked() )
    {
        ui->messagesTextEdit->append( "Buscando lugar: " + ui->searchImagesLineEdit->text() + "..." );
        webImage->searchForStreetViewImage( ui->searchImagesLineEdit->text() );
    }

    connect( webImage, SIGNAL( notImage() ), SLOT( slot_webImageNotImage() ) );
    connect( webImage, SIGNAL( imageReady( GLuint ) ), SLOT( slot_webImageReady( GLuint ) ) );
}

void MainWindow::slot_webImageNotImage()
{
    ui->messagesTextEdit->append( "Imagen: Sin resultados" );
    ui->scene->setWebImageTextureId( 0 );
    enableImagesSearch();
}

void MainWindow::slot_webImageReady( GLuint textureId )
{
    ui->messagesTextEdit->append( "Imagen descargada" );
    ui->scene->setWebImageTextureId( textureId );
    enableImagesSearch();
}

void MainWindow::slot_searchMusicButtonClicked()
{
    ui->searchMusicLineEdit->setText( ui->searchMusicLineEdit->text().simplified() );
    if( ui->searchMusicLineEdit->text() == "" )
    {
        ui->scene->setWebMusicImageTextureId( 0 );
        ui->scene->setWebMusicPlayerMedia( "" );
        return;
    }

    ui->searchMusicLineEdit->setEnabled( false );
    ui->searchMusicButton->setEnabled( false );
    ui->searchMusicButton->setText( "Buscando" );

    ui->messagesTextEdit->append( "Buscando musica: " + ui->searchMusicLineEdit->text() + "..." );

    WebMusic *webMusic = new WebMusic( this );
    webMusic->searchMusic( ui->searchMusicLineEdit->text() );

    connect( webMusic, SIGNAL( notMusic() ), SLOT( slot_webMusicNotMusic() ) );
    connect( webMusic, SIGNAL( musicReady( QString ) ), SLOT( slot_webMusicReady( QString ) ) );
    connect( webMusic, SIGNAL( musicImageReady( GLuint ) ), SLOT( slot_webMusicAssociatedImageReady( GLuint ) ) );
}

void MainWindow::slot_webMusicNotMusic()
{
    ui->messagesTextEdit->append( "Musica: Sin resultados" );
    ui->scene->setWebMusicPlayerMedia( "" );
    enableMusicSearch();
}

void MainWindow::slot_webMusicReady( QString uri )
{
    ui->messagesTextEdit->append( "Musica encontrada" );
    ui->messagesTextEdit->append( "Reproduciendo desde: " + uri );
    ui->scene->setWebMusicPlayerMedia( uri );
    enableMusicSearch();
}

void MainWindow::slot_webMusicAssociatedImageReady( GLuint textureId )
{
    ui->scene->setWebMusicImageTextureId( textureId );
}

void MainWindow::slot_detectMarkersCheckBoxValueChanged( bool checked )
{
    ui->scene->detectMarkersOption = checked;

    if( checked )
    {
        ui->rotateCheckBox->setEnabled( true );
        ui->descriptionCheckBox->setEnabled( true );
        ui->scaleLabel->setEnabled( true );
        ui->scaleSpinBox->setEnabled( true );
        ui->xTranslationLabel->setEnabled( true );
        ui->xTranslationSpinBox->setEnabled( true );
        ui->yTranslationLabel->setEnabled( true );
        ui->yTranslationSpinBox->setEnabled( true );
        ui->zTranslationLabel->setEnabled( true );
        ui->zTranslationSpinBox->setEnabled( true );
        ui->xRotationLabel->setEnabled( true );
        ui->xRotationSpinBox->setEnabled( true );
        ui->yRotationLabel->setEnabled( true );
        ui->yRotationSpinBox->setEnabled( true );
        ui->zRotationLabel->setEnabled( true );
        ui->zRotationSpinBox->setEnabled( true );
        ui->firstFaceCheckBox->setChecked( false );
    }
    else
    {
        ui->rotateCheckBox->setEnabled( false );
        ui->rotateCheckBox->setChecked( false );
        ui->descriptionCheckBox->setEnabled( false );
        ui->scaleLabel->setEnabled( false );
        ui->scaleSpinBox->setEnabled( false );
        ui->xTranslationLabel->setEnabled( false );
        ui->xTranslationSpinBox->setEnabled( false );
        ui->yTranslationLabel->setEnabled( false );
        ui->yTranslationSpinBox->setEnabled( false );
        ui->zTranslationLabel->setEnabled( false );
        ui->zTranslationSpinBox->setEnabled( false );
        ui->xRotationLabel->setEnabled( false );
        ui->xRotationSpinBox->setEnabled( false );
        ui->yRotationLabel->setEnabled( false );
        ui->yRotationSpinBox->setEnabled( false );
        ui->zRotationLabel->setEnabled( false );
        ui->zRotationSpinBox->setEnabled( false );
    }
}

void MainWindow::slot_rotateCheckBoxValueChanged( bool checked )
{
    ui->scene->rotateOption = checked;

    if( checked )
    {
        ui->rotationVelocityLabel->setEnabled( true );
        ui->rotationVelocitySlider->setEnabled( true );
        ui->zRotationLabel->setEnabled( false );
        ui->zRotationSpinBox->setEnabled( false );
    }
    else
    {
        ui->rotationVelocityLabel->setEnabled( false );
        ui->rotationVelocitySlider->setEnabled( false );
        ui->zRotationLabel->setEnabled( true );
        ui->zRotationSpinBox->setEnabled( true );
        ui->zRotationSpinBox->setValue( 0 );
        slot_zRotationSpinBoxValueChanged( ui->zRotationSpinBox->value() );
    }
}

void MainWindow::slot_rotationVelocitySliderValueChanged( int value )
{
    ui->scene->rotationVelocityOption = value;
}

void MainWindow::slot_descriptionCheckBoxValueChanged( bool checked )
{
    ui->scene->descriptionOption = checked;
}

void MainWindow::slot_scaleSpinBoxValueChanged( int value )
{
    ui->scene->scaleOption = value;
}

void MainWindow::slot_xTranslationSpinBoxValueChanged( double value )
{
    ui->scene->xTranslationOption = value;
}

void MainWindow::slot_yTranslationSpinBoxValueChanged( double value )
{
    ui->scene->yTranslationOption = value;
}

void MainWindow::slot_zTranslationSpinBoxValueChanged( double value )
{
    ui->scene->zTranslationOption = value;
}

void MainWindow::slot_xRotationSpinBoxValueChanged( int value )
{
    if( value == -360 || value == 360 ) ui->xRotationSpinBox->setValue( 0 );
    ui->scene->xRotationOption = value;
}

void MainWindow::slot_yRotationSpinBoxValueChanged( int value )
{
    if( value == -360 || value == 360 ) ui->yRotationSpinBox->setValue( 0 );
    ui->scene->yRotationOption = value;
}

void MainWindow::slot_zRotationSpinBoxValueChanged( int value )
{
    if( value == -360 || value == 360 ) ui->zRotationSpinBox->setValue( 0 );
    ui->scene->zRotationOption = value;
}

void MainWindow::slot_detectFacesCheckBoxValueChanged( bool checked )
{
    ui->scene->detectFacesOption = checked;

    if( checked )
    {
        ui->firstFaceCheckBox->setEnabled( true );
        ui->linesWidthLabel->setEnabled( true );
        ui->linesWidthSlider->setEnabled( true );
        ui->eraseMouthCheckBox->setEnabled( true );
        ui->seguirRostroCheckBox->setChecked(false);
        ui->seguirRostroCheckBox->setEnabled(true);
    }
    else
    {
        ui->firstFaceCheckBox->setEnabled( false );
        ui->firstFaceCheckBox->setChecked( false );
        ui->linesWidthLabel->setEnabled( false );
        ui->linesWidthSlider->setEnabled( false );
        ui->eraseMouthCheckBox->setEnabled( false );
        ui->eraseMouthCheckBox->setChecked(false );
        ui->detectMarkersCheckBox->setEnabled( true );
        ui->seguirRostroCheckBox->setChecked(false);
        ui->seguirRostroCheckBox->setEnabled(false);
    }
}

void MainWindow::slot_firstFaceCheckBoxValueChanged( bool checked )
{
    ui->scene->firstFaceOption = checked;

    if( checked )
    {
        ui->detectMarkersCheckBox->setChecked( false );
        ui->eraseMouthCheckBox->setChecked( false );
    }
    else
    {
        ui->detectMarkersCheckBox->setEnabled( true );
        ui->eraseMouthCheckBox->setEnabled( true );
    }
}

void MainWindow::slot_linesWidthSliderValueChanged( int value )
{
    ui->messagesTextEdit->append( "Nuevo ancho de lineas: " + QString::number( value ) );
    ui->scene->linesWidthOption = value;
}

void MainWindow::slot_eraseMouthCheckBoxValueChanged( bool checked )
{
    ui->scene->eraseMouthOption = checked;

    if( checked )
    {
        ui->firstFaceCheckBox->setChecked( false );
        ui->blurMouthLabel->setEnabled( true );
        ui->blurMouthSlider->setEnabled( true );
    }
    else
    {
        if( ui->detectFacesCheckBox->isChecked() )
            ui->firstFaceCheckBox->setEnabled( true );
        ui->blurMouthLabel->setEnabled( false );
        ui->blurMouthSlider->setEnabled( false );
    }
}

void MainWindow::slot_blurSliderValueChanged( int value )
{
    ui->messagesTextEdit->append( "Nuevo desenfoque: " + QString::number( value ) + " pixeles" );
    ui->scene->blurMouthOption = value;
}

void MainWindow::slot_seguirRostroCheckBoxValueChanged( bool checked )  {
    ui->scene->seguirRostroOption = checked;
}


void MainWindow::keyPressEvent(QKeyEvent *e)  {
    switch(e->key())  {
    case Qt::Key_Escape:  this->close();  break;
    case Qt::Key_C:  ui->scene->cambiarCamara();  break;
    case Qt::Key_T:  ui->scene->showMaximized();  break;
    case Qt::Key_B:
        (ui->scene->showInBinary) ? ui->scene->showInBinary=false : ui->scene->showInBinary=true;
        break;
    default:;
    }
}
