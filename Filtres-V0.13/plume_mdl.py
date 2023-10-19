#! /usr/bin/python3

import struct as st

import PIL
from PIL import ImageShow
import PIL.Image as Image
import PIL.ImageDraw as ImageDraw
import PIL.ImageFont as ImageFont

from subprocess import Popen, PIPE, STDOUT
from os import system

#import matplotlib.image as mpimg
#import matplotlib.pyplot as plt

from PIL import ImageShow

im = Image.new(mode='RGB', size=(50,50))
try:
	ImageShow.register(ImageShow.DisplayViewer(), 0)
	im.show()
except:
	pass
try:
	ImageShow.register(ImageShow.XVViewer(), 0)
	im.show()
except:
	pass

#ImageShow.register(ImageShow.XDGViewer(), 0)
ImageShow.register(ImageShow.XDGViewer(), 0)

from math import cos, acos, sin

ESPACE_FLTR = 10
BORD_FLTR = 2
BANDE_VIDE_FLTR = 15
HAUTEUR_FLTR = 150

HAUT_POID = 15
BORD_POID = 20

NOIR = (0,0,0)
GRIS = (100,100,100)
ROUGE = (255,0,0)
BLEU = (0,0,255)
VERT = (0,255,0)

def dessinner_le_model(
	C, Y_max, MAX_N, P,
	_type, _y, _ema, _inter, _n,
	conste, poids,
	nom_de_fichier):

	__P = int(P**.5)

	assert(_n > __P)


	DEPART = 50

	X = _y[0]*(_n*ESPACE_FLTR + 2*BANDE_VIDE_FLTR) + 2*BORD_FLTR
	Y = DEPART + HAUTEUR_FLTR + BORD_POID + (C-1) * (HAUT_POID*__P + BORD_POID*2)

	image = Image.new(
		mode="RGB",
		size=(X,Y),
		color=(255,255,255)
	)

	draw = ImageDraw.Draw(image)

	font = ImageFont.truetype("AbyssinicaSIL-Regular.ttf", 32)
	font2 = ImageFont.truetype("AbyssinicaSIL-Regular.ttf", 20)

	for i in range(_y[0]):
		draw.text(
			(i*(_n*ESPACE_FLTR + 2*BANDE_VIDE_FLTR), 0),
			f" ema = {_ema[i]}\n inter = {_inter[i]}",
			(0,0,0),
			font=font2
		)

	def ligne(x0,y0, x1,y1, couleur=NOIR):
		dx =  abs(x1-x0); sx = (1 if x0<x1 else -1);
		dy = -abs(y1-y0); sy = (1 if y0<y1 else -1);
		err = dx+dy
		e2 = 0

		while(1):
			if X>x0>=0 and Y>y0>=0:
				image.putpixel((x0,y0), couleur)
			if x0==x1 and y0==y1:
				break
			e2 = 2*err;
			if (e2 >= dy):
				err += dy
				x0 += sx
			if (e2 <= dx):
				err += dx
				y0 += sy

	def caree(x0,y0, x1,y1, couleur=NOIR):
		for x in range(x0, x1):
			for y in range(y0, y1):
				if (X>x>=0 and Y>y>=0):
					try:
						image.putpixel((x,y), couleur)
					except:
						breakpoint()

	def cercle(x,y, R, couleur=NOIR):
		for _x in range(x-R-1, x+R+2):
			for _y in range(y-R-1, y+R+2):
				if ((x-_x)**2 + (y-_y)**2)**.5 < R:
					if X>_x>=0 and Y>_y>=0:
						image.putpixel((_x,_y), couleur)

	#	Dessin des Filtres
		#	Dessin des carrees
	for i in range(_y[0]):
		L = _n*ESPACE_FLTR + 2*BANDE_VIDE_FLTR
		H = HAUTEUR_FLTR
		x,y = (i*(_n*ESPACE_FLTR + 2*BANDE_VIDE_FLTR), DEPART)

		ligne(x,y,     x+L,y,   couleur=GRIS)
		ligne(x+L,y,   x+L,y+H, couleur=GRIS)
		ligne(x+L,y+H, x,y+H,   couleur=GRIS)
		ligne(x,y+H,   x,y,     couleur=GRIS)

		dernier_point = [x+BANDE_VIDE_FLTR, DEPART + BANDE_VIDE_FLTR+int((1-conste[i*_n + 0]) * (HAUTEUR_FLTR-2*BANDE_VIDE_FLTR))]
		
		for j in range(1, _n):
			hauteur = DEPART + BANDE_VIDE_FLTR+int((1-conste[i*_n + j]) * (HAUTEUR_FLTR-2*BANDE_VIDE_FLTR))
			__x = x + BANDE_VIDE_FLTR + j*ESPACE_FLTR

			x0 = dernier_point[0]
			y0 = dernier_point[1]
			x1 = __x
			y1 = hauteur

			ligne(x0,y0, x1,y1, couleur=NOIR)
			cercle(x0,y0, R=4, couleur=NOIR)
			cercle(x1,y1, R=4, couleur=NOIR)
			dernier_point = (__x, hauteur)

		x += BORD_POID
		for c in range(1,C):
			if i % (2**c) == 0:
				
				y = DEPART + HAUTEUR_FLTR + 2*BORD_POID + (c-1)*(2*BORD_POID+HAUT_POID*__P)

				plus_x = (
					2*(_n*ESPACE_FLTR + 2*BANDE_VIDE_FLTR)*0.9
						if c == 1 else (_n*ESPACE_FLTR + 2*BANDE_VIDE_FLTR)*(2**(c-1)) + _n*ESPACE_FLTR + 2*BANDE_VIDE_FLTR
				)
				
				caree(
					x,y-2*int(BORD_POID/2) - 2,
					x+int(plus_x),y-2*int(BORD_POID/2) + 2, BLEU)

				for _i in range(__P):
					for _j in range(__P):
						#p = poids[c*Y_max*P + int(i/(2**c))*P + _i*__P+_j]
						try:
							p = poids[(2**(C-1)-2**(C-c))*__P*__P + (int(i/(2**c)))*__P*__P + _j*7 + _i]
						except:
							breakpoint()
						caree(
							x+_i*HAUT_POID,y+_j*HAUT_POID, 
							x+(_i+1)*HAUT_POID, y+(_j+1)*HAUT_POID,
							couleur=(
								255-int(255*(1+p)/2),
								int((1+p)/2*255),
								0
							)
						)
				
				draw.text(
					(x+__P*HAUT_POID + 5, y),
					f"#{c*Y_max*P + int(i/(2**c))*P + _i*__P+_j}",
					(0,0,0),
					font=font
				)
		
	image.show()

def lire_uint(text, N):
	s = st.unpack('I'*N, text[:N*4])
	return s, text[4*N:]

def lire_flotants(text, N):
	s = st.unpack('f'*N, text[:4*N])
	return s, text[4*N:]

def faire_la_plume(fichier):
	with open(fichier, 'rb') as co:
		text = co.read()

		(C, Y_max, MAX_N, P),text = lire_uint(text, 4)
		_P = int((P)**.5)
		
		lignes, text = lire_uint(text, Y_max*3)
		poids, text = lire_flotants(text, _P*_P*(Y_max-1))
		conste, text = lire_flotants(text, Y_max*MAX_N)

		_type = [0,1,1,1,1,1]
		_ema = [lignes[i*3+1] for i in range(int(len(lignes)/3))]
		_inter = [lignes[i*3+2] for i in range(int(len(lignes)/3))]
		_y = 32, 16, 8, 4, 2, 1
		_n = MAX_N

		del text

	dessinner_le_model(
		C, Y_max, MAX_N, P,
		_type, _y, _ema, _inter, _n,
		conste, poids, fichier)

if __name__ == "__main__":
	from sys import argv
	fichier = argv[1]

	effacer = 0

	try:
		faire_la_plume(fichier)
	except Exception as exception:
		from os import system
		system(f"sudo rm {fichier}")
		effacer = 1
		raise(exception)
	
	if effacer == 0:
		from os import system
		system(f"sudo rm {fichier}")