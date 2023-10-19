from os import listdir
import json
from os import system

COMPILATEUR = [
	'gcc',
	'clang'
][0]#, 'zig', 'clang'

def lire_somme(chemin):
	with open(chemin, 'rb') as co:
		elm = co.read()
		return [(i+1)*elm[i] for i in range(len(elm))]

def fichiers(chemins, terminaison='.h'):
	f = []
	for ch in chemins:
		for _f in listdir():
			if _f[-2:]==terminaison:
				f += [(ch+'/'+_f).replace('//', '/')]
##=================================

chemins_def = [
	'def/',
	'def/main/'
]
fichiers_tete = [
	chemin+fichier
		for chemin in chemins_def
			for fichier in listdir(chemin)
				if fichier[-2:]=='.h'
]

##=================================

chemins_impl = [
	'impl/',
	'impl/etc/',

	'impl/mdl/',
	'impl/mdl/f/'
]
fichiers_impl = [
	chemin+fichier
		for chemin in chemins_impl
			for fichier in listdir(chemin)
				if fichier[-2:]=='.c'
]

##=================================

sommes_impl = {fichier : lire_somme(fichier) for fichier in fichiers_impl}
sommes_tete = {fichier : lire_somme(fichier) for fichier in fichiers_tete}

a_compiler = []

if not 'trace_tete' in listdir():
	#	Pas de trace de la tete, on l'ecrit et on compile tout
	a_compiler = fichiers_impl
	with open('trace_tete', 'w') as co: co.write(json.dumps(sommes_tete))
	with open('trace', 'w') as co: co.write(json.dumps(sommes_impl))
else:
	with open('trace_tete', 'r') as co: sommes_de_trace_tete = eval(co.read())
	with open('trace', 'r') as co: sommes_de_trace_impl = eval(co.read())
	#print(sommes_tete, sommes_de_trace_tete)
	
	if not all(tete in sommes_de_trace_tete.keys() for tete in fichiers_tete):
		a_compiler = fichiers_impl
	elif not all(impl in sommes_de_trace_impl.keys() for impl in fichiers_impl):
		a_compiler = fichiers_impl
	elif not all(sommes_de_trace_tete[tete] == sommes_tete[tete] for tete in fichiers_tete):
		a_compiler = fichiers_impl
	elif not 'trace' in listdir():
		#	Pas de fichier de trace on compile tout et on ecrit la trace
		with open('trace', 'w') as co: co.write(json.dumps(sommes_impl))
		a_compiler = fichiers_impl
	else:
		# Fichier trace -> on recompile que les fichier modifiees
		with open('trace', 'r') as co: sommes_de_trace = eval(co.read())
		for fichier,_somme in sommes_impl.items():
			if not fichier in sommes_de_trace.keys():
				a_compiler += [fichier]
			elif sommes_de_trace[fichier] != _somme:
				a_compiler += [fichier]
			#else:
			#	on recompile pas

if not 'prog' in listdir():
	a_compiler = sommes_impl

SDL_FLAGS = " -g -O3 -mavx -lm -lpthread -D_REENTRANT -fopenmp -Wall" #"-O3" #" -mavx -g" #"-lSDL2 -lSDL2main -lSDL2_ttf `sdl2-config --cflags --libs`"

print(f"[***] {len(a_compiler)}/{len(sommes_impl)} fichiers a compiler")
if len(a_compiler) == 0:
	exit(0)
for i,_a_compiler in enumerate(a_compiler):
	system(f'printf "\\rCompilation : %3d%% [%.*s%*s]" {int((i+1)/len(a_compiler)*100)} {i} "{"#"*len(a_compiler)}" {len(a_compiler)-i-1} ""')

	nom_fichier = _a_compiler.split('/')[-1]
	e = system(f"{COMPILATEUR} -c {_a_compiler} -Idef" + SDL_FLAGS)
	if e != 0: exit(1)
	system(f"mv {nom_fichier.replace('.c','.o')} bin/{_a_compiler.replace('/','_').replace('.c','.o')}")
print("")

system("rm prog")
system(f"{COMPILATEUR} bin/*.o -o prog -Idef " + SDL_FLAGS)

##	Mise a jour si tout se passe bien
sommes_impl = {fichier : lire_somme(fichier) for fichier in fichiers_impl}
sommes_tete = {fichier : lire_somme(fichier) for fichier in fichiers_tete}
with open('trace_tete', 'w') as co: co.write(json.dumps(sommes_tete))
with open('trace', 'w') as co: co.write(json.dumps(sommes_impl))
