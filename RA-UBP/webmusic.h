#ifndef WEBMUSIC_H
#define WEBMUSIC_H

#include "webimage.h"

class WebMusic : public QObject
{
    Q_OBJECT

private:

    QNetworkAccessManager *sourceCodeManager;
    QString artist, title;
    WebImage *associatedImage;

    void replaceCharacters( QString &text )
    {
        text.replace( "&lsquo;", "‘" );     text.replace( "&ndash;", "–" );     text.replace( "&Uuml;", "Ü" );
        text.replace( "&rsquo;", "’" );     text.replace( "&#150;", "–" );      text.replace( "&#220;", "Ü" );
        text.replace( "&sbquo;", "‚" );     text.replace( "&mdash;", "—" );     text.replace( "&Yacute;", "Ý" );
        text.replace( "&ldquo;", "“" );     text.replace( "&#151;", "—" );      text.replace( "&#221;", "Ý" );
        text.replace( "&rdquo;", "”" );     text.replace( "&nbsp;", " " );      text.replace( "&THORN;", "Þ" );
        text.replace( "&bdquo;", "„" );     text.replace( "&#160;", " " );      text.replace( "&#222;", "Þ" );
        text.replace( "&dagger;", "†" );	text.replace( "&iexcl;", "¡" );     text.replace( "&szlig;", "ß" );
        text.replace( "&Dagger;", "‡" );	text.replace( "&#161;", "¡" );      text.replace( "&#223;", "ß" );
        text.replace( "&permil;", "‰" );	text.replace( "&cent;", "¢" );      text.replace( "&agrave;", "à" );
        text.replace( "&lsaquo;", "‹" );	text.replace( "&#162;", "¢" );      text.replace( "&#224;", "à" );
        text.replace( "&rsaquo;", "›" );	text.replace( "&pound;", "£" );     text.replace( "&aacute;", "á" );
        text.replace( "&spades;", "♠" );	text.replace( "&#163;", "£" );      text.replace( "&#225;", "á" );
        text.replace( "&clubs;", "♣" );     text.replace( "&curren;", "¤" );	text.replace( "&acirc;", "â" );
        text.replace( "&hearts;", "♥" );	text.replace( "&#164;", "¤" );      text.replace( "&#226;", "â" );
        text.replace( "&diams;", "♦" );     text.replace( "&yen;", "¥" );       text.replace( "&atilde;", "ã" );
        text.replace( "&oline;", "‾" );     text.replace( "&#165;", "¥" );      text.replace( "&#227;", "ã" );
        text.replace( "&larr;", "←" );      text.replace( "&brvbar;", "¦" );	text.replace( "&auml;", "ä" );
        text.replace( "&uarr;", "↑" );      text.replace( "&brkbar;", "¦" );	text.replace( "&#228;", "ä" );
        text.replace( "&rarr;", "→" );      text.replace( "&#166;", "¦" );      text.replace( "&aring;", "å" );
        text.replace( "&darr;", "↓" );      text.replace( "&sect;", "§" );      text.replace( "&#229;", "å" );
        text.replace( "&trade;", "™" );     text.replace( "&#167;", "§" );      text.replace( "&aelig;", "æ" );
        text.replace( "&#x2122;", "™" );	text.replace( "&uml;", "¨" );       text.replace( "&#230;", "æ" );
        text.replace( "&#32;", " " );       text.replace( "&die;", "¨" );       text.replace( "&ccedil;", "ç" );
        text.replace( "&#33;", "!" );       text.replace( "&#168;", "¨" );      text.replace( "&#231;", "ç" );
        text.replace( "&quot;", "\"" );     text.replace( "&copy;", "©" );      text.replace( "&egrave;", "è" );
        text.replace( "&#34;", "\"" );      text.replace( "&#169;", "©" );      text.replace( "&#232;", "è" );
        text.replace( "&#35;", "#" );       text.replace( "&ordf;", "ª" );      text.replace( "&eacute;", "é" );
        text.replace( "&#36;", "$" );       text.replace( "&#170;", "ª" );      text.replace( "&#233;", "é" );
        text.replace( "&#37;", "%" );       text.replace( "&laquo;", "«" );     text.replace( "&ecirc;", "ê" );
        text.replace( "&amp;", "&" );       text.replace( "&#171;", "«" );      text.replace( "&#234;", "ê" );
        text.replace( "&#38;", "&" );       text.replace( "&not;", "¬" );       text.replace( "&euml;", "ë" );
        text.replace( "&#39;", "’" );       text.replace( "&#172;", "¬" );      text.replace( "&#235;", "ë" );
        text.replace( "&#40;", "(" );       text.replace( "&reg;", "®" );       text.replace( "&igrave;", "ì" );
        text.replace( "&#41;", ")" );       text.replace( "&#174;", "®" );      text.replace( "&#236;", "ì" );
        text.replace( "&#42;", "*" );       text.replace( "&macr;", "¯" );      text.replace( "&iacute;", "í" );
        text.replace( "&#43;", "+" );       text.replace( "&hibar;", "¯" );     text.replace( "&#237;", "í" );
        text.replace( "&#44;", "," );       text.replace( "&#175;", "¯" );      text.replace( "&icirc;", "î" );
        text.replace( "&#45;", "-" );       text.replace( "&deg;", "°" );       text.replace( "&#238;", "î" );
        text.replace( "&#46;", "." );       text.replace( "&#176;", "°" );      text.replace( "&iuml;", "ï" );
        text.replace( "&frasl;", "/" );     text.replace( "&plusmn;", "±" );	text.replace( "&#239;", "ï" );
        text.replace( "&#47;", "/" );       text.replace( "&#177;", "±" );      text.replace( "&eth;", "ð" );
        text.replace( "&#48;", "0" );       text.replace( "&sup2;", "²" );      text.replace( "&#240;", "ð" );
        text.replace( "&#49;", "1" );       text.replace( "&#178;", "²" );      text.replace( "&ntilde;", "ñ" );
        text.replace( "&#50;", "2" );       text.replace( "&sup3;", "³" );      text.replace( "&#241;", "ñ" );
        text.replace( "&#51;", "3" );       text.replace( "&#179;", "³" );      text.replace( "&ograve;", "ò" );
        text.replace( "&#52;", "4" );       text.replace( "&acute;", "´" );     text.replace( "&#242;", "ò" );
        text.replace( "&#53;", "5" );       text.replace( "&#180;", "´" );      text.replace( "&oacute;", "ó" );
        text.replace( "&#54;", "6" );       text.replace( "&#micro;", "µ" );	text.replace( "&#243;", "ó" );
        text.replace( "&#55;", "7" );       text.replace( "&#181;", "µ" );      text.replace( "&ocirc;", "ô" );
        text.replace( "&#56;", "8" );       text.replace( "&para;", "¶" );      text.replace( "&#244;", "ô" );
        text.replace( "&#57;", "9" );       text.replace( "&#182;", "¶" );      text.replace( "&otilde;", "õ" );
        text.replace( "&#58;", ":" );       text.replace( "&middot;", "·" );	text.replace( "&#245;", "õ" );
        text.replace( "&#59;", ";" );       text.replace( "&#183;", "·" );      text.replace( "&ouml;", "ö" );
        text.replace( "&lt;", "<" );        text.replace( "&cedil;", "¸" );     text.replace( "&#246;", "ö" );
        text.replace( "&#60;", "<" );       text.replace( "&#184;", "¸" );      text.replace( "&divide;", "÷" );
        text.replace( "&#61;", "=" );       text.replace( "&sup1;", "¹" );      text.replace( "&#247;", "÷" );
        text.replace( "&gt;", ">" );        text.replace( "&#185;", "¹" );      text.replace( "&oslash;", "ø" );
        text.replace( "&#62;", ">" );       text.replace( "&ordm;", "º" );      text.replace( "&#248;", "ø" );
        text.replace( "&#63;", "?" );       text.replace( "&#186;", "º" );      text.replace( "&ugrave;", "ù" );
        text.replace( "&#64;", "@" );       text.replace( "&raquo;", "»" );     text.replace( "&#249;", "ù" );
        text.replace( "&#65;", "A" );       text.replace( "&#187;", "»" );      text.replace( "&uacute;", "ú" );
        text.replace( "&#66;", "B" );       text.replace( "&frac14;", "¼" );	text.replace( "&#250;", "ú" );
        text.replace( "&#67;", "C" );       text.replace( "&#188;", "¼" );      text.replace( "&ucirc;", "û" );
        text.replace( "&#68;", "D" );       text.replace( "&frac12;", "½" );	text.replace( "&#251;", "û" );
        text.replace( "&#69;", "E" );       text.replace( "&#189;", "½" );      text.replace( "&uuml;", "ü" );
        text.replace( "&#70;", "F" );       text.replace( "&frac34;", "¾" );	text.replace( "&#252;", "ü" );
        text.replace( "&#71;", "G" );       text.replace( "&#190;", "¾" );      text.replace( "&yacute;", "ý" );
        text.replace( "&#72;", "H" );       text.replace( "&iquest;", "¿" );	text.replace( "&#253;", "ý" );
        text.replace( "&#73;", "I" );       text.replace( "&#191;", "¿" );      text.replace( "&thorn;", "þ" );
        text.replace( "&#74;", "J" );       text.replace( "&Agrave;", "À" );	text.replace( "&#254;", "þ" );
        text.replace( "&#75;", "K" );       text.replace( "&#192;", "À" );      text.replace( "&yuml;", "ÿ" );
        text.replace( "&#76;", "L" );       text.replace( "&Aacute;", "Á" );	text.replace( "&#255;", "ÿ" );
        text.replace( "&#77;", "M" );       text.replace( "&#193;", "Á" );      text.replace( "&Alpha;", "Α" );
        text.replace( "&#78;", "N" );       text.replace( "&Acirc;", "Â" );     text.replace( "&alpha;", "α" );
        text.replace( "&#79;", "O" );       text.replace( "&#194;", "Â" );      text.replace( "&Beta;", "Β" );
        text.replace( "&#80;", "P" );       text.replace( "&Atilde;", "Ã" );	text.replace( "&beta;", "β" );
        text.replace( "&#81;", "Q" );       text.replace( "&#195;", "Ã" );      text.replace( "&Gamma;", "Γ" );
        text.replace( "&#82;", "R" );       text.replace( "&Auml;", "Ä" );      text.replace( "&gamma;", "γ" );
        text.replace( "&#83;", "S" );       text.replace( "&#196;", "Ä" );      text.replace( "&Delta;", "Δ" );
        text.replace( "&#84;", "T" );       text.replace( "&Aring;", "Å" );     text.replace( "&delta;", "δ" );
        text.replace( "&#85;", "U" );       text.replace( "&#197;", "Å" );      text.replace( "&Epsilon;", "Ε" );
        text.replace( "&#86;", "V" );       text.replace( "&AElig;", "Æ" );     text.replace( "&epsilon;", "ε" );
        text.replace( "&#87;", "W" );       text.replace( "&#198;", "Æ" );      text.replace( "&Zeta;", "Ζ" );
        text.replace( "&#88;", "X" );       text.replace( "&Ccedil;", "Ç" );	text.replace( "&zeta;", "ζ" );
        text.replace( "&#89;", "Y" );       text.replace( "&#199;", "Ç" );      text.replace( "&Eta;", "Η" );
        text.replace( "&#90;", "Z" );       text.replace( "&Egrave;", "È" );	text.replace( "&eta;", "η" );
        text.replace( "&#91;", "[" );       text.replace( "&#200;", "È" );      text.replace( "&Theta;", "Θ" );
        text.replace( "&#93;", "]" );       text.replace( "&Eacute;", "É" );	text.replace( "&theta;", "θ" );
        text.replace( "&#94;", "^" );       text.replace( "&#201;", "É" );      text.replace( "&Iota;", "Ι" );
        text.replace( "&#95;", "_" );       text.replace( "&Ecirc;", "Ê" );     text.replace( "&iota;", "ι" );
        text.replace( "&#96;", "`" );       text.replace( "&#202;", "Ê" );      text.replace( "&Kappa;", "Κ" );
        text.replace( "&#97;", "a" );       text.replace( "&Euml;", "Ë" );      text.replace( "&kappa;", "κ" );
        text.replace( "&#98;", "b" );       text.replace( "&#203;", "Ë" );      text.replace( "&Lambda;", "Λ" );
        text.replace( "&#99;", "c" );       text.replace( "&Igrave;", "Ì" );	text.replace( "&lambda;", "λ" );
        text.replace( "&#100;", "d" );      text.replace( "&#204;", "Ì" );      text.replace( "&Mu;", "Μ" );
        text.replace( "&#101;", "e" );      text.replace( "&Iacute;", "Í" );	text.replace( "&mu;", "μ" );
        text.replace( "&#102;", "f" );      text.replace( "&#205;", "Í" );      text.replace( "&Nu;", "Ν" );
        text.replace( "&#103;", "g" );      text.replace( "&Icirc;", "Î" );     text.replace( "&nu;", "ν" );
        text.replace( "&#104;", "h" );      text.replace( "&#206;", "Î" );      text.replace( "&Xi;", "Ξ" );
        text.replace( "&#105;", "i" );      text.replace( "&Iuml;", "Ï" );      text.replace( "&xi;", "ξ" );
        text.replace( "&#106;", "j" );      text.replace( "&#207;", "Ï" );      text.replace( "&Omicron;", "Ο" );
        text.replace( "&#107;", "k" );      text.replace( "&ETH;", "Ð" );       text.replace( "&omicron;", "ο" );
        text.replace( "&#108;", "l" );      text.replace( "&#208;", "Ð" );      text.replace( "&Pi;", "Π" );
        text.replace( "&#109;", "m" );      text.replace( "&Ntilde;", "Ñ" );	text.replace( "&pi;", "π" );
        text.replace( "&#110;", "n" );      text.replace( "&#209;", "Ñ" );      text.replace( "&Rho;", "Ρ" );
        text.replace( "&#111;", "o" );      text.replace( "&Ograve;", "Ò" );	text.replace( "&rho;", "ρ" );
        text.replace( "&#112;", "p" );      text.replace( "&#210;", "Ò" );      text.replace( "&Sigma;", "Σ" );
        text.replace( "&#113;", "q" );      text.replace( "&Oacute;", "Ó" );	text.replace( "&sigma;", "σ" );
        text.replace( "&#114;", "r" );      text.replace( "&#211;", "Ó" );      text.replace( "&Tau;", "Τ" );
        text.replace( "&#115;", "S" );      text.replace( "&Ocirc;", "Ô" );     text.replace( "&tau;", "τ" );
        text.replace( "&#116;", "t" );      text.replace( "&#212;", "Ô" );      text.replace( "&Upsilon;", "Υ" );
        text.replace( "&#117;", "u" );      text.replace( "&Otilde;", "Õ" );	text.replace( "&upsilon;", "υ" );
        text.replace( "&#118;", "v" );      text.replace( "&#213;", "Õ" );      text.replace( "&Phi;", "Φ" );
        text.replace( "&#119;", "w" );      text.replace( "&Ouml;", "Ö" );      text.replace( "&phi;", "φ" );
        text.replace( "&#120;", "x" );      text.replace( "&#214;", "Ö" );      text.replace( "&Chi;", "Χ" );
        text.replace( "&#121;", "y" );      text.replace( "&times;", "×" );     text.replace( "&chi;", "χ" );
        text.replace( "&#122;", "z" );      text.replace( "&#215;", "×" );      text.replace( "&Psi;", "Ψ" );
        text.replace( "&#123;", "{" );      text.replace( "&Oslash;", "Ø" );	text.replace( "&psi;", "ψ" );
        text.replace( "&#124;", "|" );      text.replace( "&#216;", "Ø" );      text.replace( "&Omega;", "Ω" );
        text.replace( "&#125;", "}" );      text.replace( "&Ugrave;", "Ù" );	text.replace( "&omega;", "ω" );
        text.replace( "&#126;", "~" );      text.replace( "&#217;", "Ù" );      text.replace( "&#9679;", "●" );
        text.replace( "&hellip;", "…" );	text.replace( "&Uacute;", "Ú" );	text.replace( "&#8226;", "•" );
        text.replace( "&#133;", "…" );      text.replace( "&#218;", "Ú" );      text.replace( "_", " " );
        text.replace( "&Ucirc;", "Û" );     text.replace( "&#219;", "Û" );
    }

public:

    WebMusic( QObject *parent = 0 ) : QObject( parent ),
                                      sourceCodeManager( new QNetworkAccessManager( this ) ),
                                      artist( "" ),
                                      title( "" ),
                                      associatedImage( new WebImage( this ) )
    {
        connect( sourceCodeManager, SIGNAL( finished( QNetworkReply* ) ),
                 SLOT( slot_sourceCodeDownloaded( QNetworkReply* ) ) );

        connect( associatedImage, SIGNAL( notImage() ),
                 SLOT( slot_associatedImageNotImage() ) );

        connect( associatedImage, SIGNAL( imageReady( GLuint ) ),
                 SLOT( slot_associatedImageReady( GLuint ) ) );

        connect( associatedImage, SIGNAL( imageWithMusicInfoReady( GLuint ) ),
                 SLOT( slot_associatedImageWithMusicInfoReady( GLuint ) ) );
    }

    void searchMusic( QString search )
    {
        sourceCodeManager->get( QNetworkRequest( QUrl( "http://taringamp3.com/descargar-musica/" + search + "/" ) ) );
        associatedImage->searchForAllImages( search );
    }

private slots:

    void slot_sourceCodeDownloaded( QNetworkReply *reply )
    {
        QString sourceCode = QString::fromUtf8( reply->readAll() ),
                preUriPlayCode = "data-sound-url=\"", endUriPlayCode = ".mp3\"",
                preArtistCode = "<i class=\"cplayer-data-sound-author\">",
                postArtistCode = "</i><b class=\"cplayer-data-sound-title\">",
                preTitleCode = "</i><b class=\"cplayer-data-sound-title\">",
                postTitleCode = "</b></h4><em class=\"cplayer-data-sound-time\">";

        QString uriPlay = "";

        if( sourceCode.count( preUriPlayCode ) > 0 && sourceCode.count( endUriPlayCode ) > 0 &&
            sourceCode.count( preArtistCode ) > 0 && sourceCode.count( postArtistCode ) > 0 &&
            sourceCode.count( preTitleCode ) > 0 && sourceCode.count( postTitleCode ) > 0 )
        {
            sourceCode.remove( 0, sourceCode.indexOf( preUriPlayCode ) + preUriPlayCode.size() );
            uriPlay = sourceCode.mid( 0, sourceCode.indexOf( endUriPlayCode ) + endUriPlayCode.size() - 1 );

            sourceCode.remove( 0, sourceCode.indexOf( preArtistCode ) + preArtistCode.size() );
            artist = sourceCode.mid( 0, sourceCode.indexOf( postArtistCode ) );
            replaceCharacters( artist );

            sourceCode.remove( 0, sourceCode.indexOf( preTitleCode ) + preTitleCode.size() );
            title = sourceCode.mid( 0, sourceCode.indexOf( postTitleCode ) );
            replaceCharacters( title );
        }

        if( !uriPlay.isEmpty() &&  !artist.isEmpty() && !title.isEmpty() )
        {
            associatedImage->drawMusicInfo( artist, title );
            emit musicReady( uriPlay );
        }
        else emit notMusic();
    }

    void slot_associatedImageNotImage() { associatedImage->loadFromLoalImage( "../Textures/Music.jpg" ); }
    void slot_associatedImageReady( GLuint ) { associatedImage->drawMusicInfo( artist, title ); }
    void slot_associatedImageWithMusicInfoReady( GLuint textureId ) { emit musicImageReady( textureId ); }

signals:

    void notMusic();
    void musicReady( QString );
    void musicImageReady( GLuint );
};

#endif // WEBMUSIC_H
