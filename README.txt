Tema            | A
	 de			| P
		casa 1	| D

Pentru inceput am folosit drept schelet varianta secventiala.
	am optat pentru versiunea cu variabile globale pentru fiecare algoritm in parte
	(variabilele pentru algoritmul julia avand indicele J si repectiv M .)
	pentru a pasa toate argumentele in functia mea.

Citirea datelor de intrare si calculul parametrilor il efectuez in main,
	inainte de a apela functia pe thread , pentru ca nu este nevoie
	ca fiecare thread sa faca citirea si in special sa recalculeze iar height,width etc.

Pentru a paraleliza algoritmul Julia si Mandelbrot, am paralizat for-ul exterior 
	la ambele calculand dupa formula utilizata si la laborator: 
	
	int start = ID * (double)widthJ / P;(respectiv widthM)
	int end = min((ID + 1) * (double)widthJ / P, widthJ); (respectiv widthM)

	pentru a imparti in mod aproximativ egal workload-ul fiecarui thread.

Pentru a paraleliza etapa de transformare a coordonatelor matematice in coordonate reale
	am pornit de la aceeasi formula si am dedus urmatoarea

	int startReverseM = thread_id * (double)(heightM / 2) / P; (respectiv heightJ)
    int endReverseM = min((thread_id + 1) * (double) (heightM / 2)/ P, heightM / 2); (respectiv heightJ)

    pentru a imparti in mod aproximativ egal workload-ul fiecarui thread.Pentru a ma asigura ca 
    operatiile de transformare se fac in ordine , am avut nevoie de o bariera linia 160 respectiv 199.	

Pentru a ma asigura ca se termina de scris in fisier bine am pus conditia ca un singur thread 
	sa faca scrierea in fisirer, si dupa algoritmul Julia am folosit o bariera din nou pentru 
	a ma asigura ca se termina scrierea pana se trece la executarea algoritmului Mandelbrot.