/* Exo1.c */

#include <X11/Xlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#define CWOverrideRedirect (1L<<9)



Display	      *dpy;			/* le serveur			     */
int	       ecran;			/* son ecran par defaut		     */
GC             gc;
Window	       racine, principale, fille[2], menu;
char          *texte_fille[4] = { "Fille 1", "Fille 2", "Degrouper", "Grouper" };
unsigned long  noir, blanc, rouge, vert, bleu;
XEvent	       evmt;
XFontStruct   *font_attributs;
Bool           grouper = False;

/* Les prototypes de fonctions */
void Installer		 (void);
void PourConfigureNotify (XConfigureEvent *evmt);
void PourExpose          (XExposeEvent *evmt);
void largeur_hauteur     (XFontStruct *font_attributs,
			  char *chaine, int *largeur, int *hauteur);
void PourButtonPress     (XButtonEvent *evmt);

int main (int argc, char *argv[]) { /* la procedue main()                */
  Installer();
  for (;;) {			    /* la boucle d'evenements            */
    XNextEvent(dpy, &evmt);
    switch (evmt.type) {
      case ConfigureNotify :
	PourConfigureNotify((XConfigureEvent *)(&evmt));
	break;
      case Expose :
	PourExpose((XExposeEvent *)(&evmt));
	break;
      case ButtonPress:
	PourButtonPress((XButtonEvent *)(&evmt));
	break;
      default :;
    }
  }
}


void Installer (void) {
  dpy            = XOpenDisplay(NULL);
  racine         = DefaultRootWindow(dpy);
  ecran          = DefaultScreen(dpy);
  gc             = DefaultGC(dpy, ecran);

  /* N'hesiter pas a utiliser la commande xfontsel pour choisir un font. */
  font_attributs = XLoadQueryFont(dpy,"-adobe-times-*-i-*-*-25-*-*-*-*-*-*-*");

  XSetFont(dpy, gc, font_attributs->fid);

  /* ne fonctionne que si le serveur est en TrueColor ou DirectColor.
     Sinon, il faut s'adresser a la table de couleur Colormap du serveur */
  noir  = 0x0;
  blanc = 0xFFFFFF;
  rouge = 0xFF0000;
  vert  = 0x00FF00;
  bleu  = 0x0000FF;
  menu   = XCreateSimpleWindow(dpy, racine, 0, 0, 120, 60, 0,
			       noir, blanc);
  principale = XCreateSimpleWindow(dpy, racine, 0, 0, 200, 200, 0,
		 noir, blanc);
  fille[0]   = XCreateSimpleWindow(dpy, principale, 0, 0, 100, 100, 1,
		 noir, rouge);
  fille[1]   = XCreateSimpleWindow(dpy, principale, 0, 100, 100, 100, 1,
		 noir, vert);



  XSetWindowAttributes a;
  a.override_redirect = True;
  XChangeWindowAttributes(dpy, menu, CWOverrideRedirect, &a);

  XSelectInput(dpy, principale, StructureNotifyMask | ButtonPressMask);
  XSelectInput(dpy, fille[0], ExposureMask);
  XSelectInput(dpy, fille[1], ExposureMask);
  XSelectInput(dpy, menu, ExposureMask | ButtonReleaseMask | ButtonPressMask);

  XStoreName(dpy, principale, "Exercice 1");
  XMapWindow(dpy, principale);
  XMapSubwindows(dpy, principale);
}


void PourConfigureNotify (XConfigureEvent *evmt) {
  int largeur, hauteur;

  largeur = evmt->width;
  hauteur = evmt->height;

  XMoveResizeWindow(dpy, fille[0], 0, 0, largeur/2, hauteur/2);
  XMoveResizeWindow(dpy, fille[1],
		    0, hauteur/2, largeur/2, hauteur - hauteur/2);
}


void PourExpose (XExposeEvent *evmt) {
  Window        racine;
  int           x, y;
  unsigned int  largeur, hauteur, largeur_bordure, profondeur;
  char         *chaine, *chaine1, *chaine2;
  int           largeur_chaine, hauteur_chaine;


    XGetGeometry(dpy, evmt->window,
		 &racine, &x, &y, &largeur, &hauteur,
		 &largeur_bordure, &profondeur);
    XClearWindow(dpy, evmt->window);

    chaine = texte_fille[evmt->window == fille[0] ? 0 : 1];
    largeur_hauteur(font_attributs, chaine, &largeur_chaine, &hauteur_chaine);
    XDrawString(dpy, evmt->window, gc,
		(largeur - largeur_chaine)/2, (hauteur + hauteur_chaine)/2,
		chaine, strlen(chaine));

  if(evmt->window == menu){

    XGetGeometry(dpy, evmt->window,
		 &racine, &x, &y, &largeur, &hauteur,
		 &largeur_bordure, &profondeur);
    XClearWindow(dpy, evmt->window);

    chaine1 = texte_fille[2]; //Grouper
    chaine2 = texte_fille[3]; //Degrouper
    largeur_hauteur(font_attributs, chaine1, &largeur_chaine, &hauteur_chaine);
    XDrawString(dpy, evmt->window, gc,
		(largeur - largeur_chaine)/2, (hauteur + hauteur_chaine)*2/3,
		chaine1, strlen(chaine1)); //Grouper
    XDrawString(dpy, evmt->window, gc,
		(largeur - largeur_chaine)/2, (hauteur + hauteur_chaine)/3,
		chaine2, strlen(chaine2)); //Degrouper
    XDrawLine(dpy, evmt->window, gc, 0, hauteur/2, largeur, hauteur/2);
  }
}


void largeur_hauteur (XFontStruct *font_attributs,
		      char *chaine, int *largeur, int *hauteur) {

  /* On recupere la largeur de la chaine avec ce font (de ce gc) */
  *largeur = XTextWidth(font_attributs, chaine, strlen(chaine));

  /* Et la hauteur (se dit ascent (haut) et descent (bas)) des caracteres */
  *hauteur = font_attributs->ascent;
}

void PourButtonPress(XButtonEvent *evmt){
  if(evmt->window == menu){

    if(evmt->x > 25){
      if(grouper == False){
	XReparentWindow(dpy, fille[0], racine, 0, 0);
	XReparentWindow(dpy, fille[1], racine, 100, 100);
	grouper=True;
      }
    }
    else{
      if(grouper == True){
	XReparentWindow(dpy, fille[0], principale, 0, 0);  //faut changer les positions
	XReparentWindow(dpy, fille[1], principale, 0, 100);
	grouper=False;
      }
    }
  }
  else if(evmt->window == principale/* && evmt->state == 3*/){
    XMapWindow(dpy, menu);
    XMoveWindow(dpy, menu, evmt->x_root, evmt->y_root);
    XWindowChanges a;
    a.stack_mode=0;
    XConfigureWindow(dpy, menu, CWStackMode, &a);
  }


}
