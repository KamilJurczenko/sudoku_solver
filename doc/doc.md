Autoren:
	Martin Klose
	Florian Neufeld
	Konstantin Daitche
	Kamil Jurczenko
Sprecher:
    Kamil Jurczenko 


Dokumentation:
	Ein Sudokufeld wird mithilfe des Backtracking Algorithmus (äquivalent zu einer Tiefensuche eines Graphens) gelöst. Hierfür wird sudoku_seriell.c von "Pradeep Mondal P" verwendet [2], wobei ein Sudokufeld als 9x9 Matrix betrachtet wird.
	Der Algorithmus verläuft folglich als Pseudocode:
	
	(1) solveSudoku aufrufen
		(2) Abbruchbedingung: 1 Ausgeben falls im letzten Feld 
		(3) Gehe Feld weiter, falls Feld keine Null (Nur im ersten solveSudoku Aufruf relevant)
		(4) Iteriere durch i <= 9 und prüfe ob i in Feld passt (legal nach Sudokuregel ist)
			(4.1) Wenn legal gebe 1 aus und gehe eine Spalte weiter und führe (1) rekursiv aus
			(4.2) Gebe 0 aus falls keine Lösung gefunden
	
	Dieser Algorithmus hat im schlimmsten Fall eine exponentielle Laufzeit von z^N und ist dementsprechend sehr Aufwendig.
	Dieses Problem lässt sich unter Umständen parallelisieren. Da der Algorithmus rekursiv verläuft, wird eine Parallelisierung der Lösungsfindung problematisch, weshalb alternative Lösungen gefunden werden müssen.
	Um dieses Problem zu lösen wurden zwei Gruppen gebildet die sich unabhängig voneinander eine Strategie zur Parallelisierung entwickelten.
	Es wurden folgende Strategien entwickelt:
	
	Strategien:
		Knoten := Startsudokus
		(1) Bildung von Startsudokus für je ein Prozess (siehe Grupp1 Branch im git)
				- Der serielle Code wurde dafür modifiziert
					- Startsudokus werden erstellt, indem eine Breitensuche n Prozesse mal, vor der eigentlichen Parallelisierung, durchgeführt wird. In jedem Durchlauf wird ein
					  Startsudoku gefunden welcher einem Prozess zugewiesen wird. Es musst so oft gesucht werden, um eine Tiefe = Prozessanzahl im Graphen zu erreichen.
				- Falls eine Lösung in einem beliebigen Prozess gefunden wurde, werden die restlichen Prozesse frühzeitig abgebrochen und das Programm wird beendet.
				  Das gelöste Sudoku und die benötigte Zeit für die Berechnung wird ausgegeben.
			mögliche Probleme:
				- Was macht ein Prozess der frühzeitig abgebrochen wird, weil er keine Lösung gefunden hat?
					- Vermutung: Sollte nur bei sehr vielen Prozessen problematisch sein.
				- Es werden eine maximale Anzahl von Prozessen benötigt, weil beispielsweise in einem Sudoku nur maximal 50 Knoten in einer Tiefe im Graphen vorkommen können[3].
			Probleme:
				- Wenn die Anzahl von Knoten in einer Tiefe im Graphen niemals genau mit der angegebennen Prozessanzahl zustimmt,
					-> Nehme die erste Tiefe die mindestens soviele Knoten hat wie Anzahl Prozesse und ignoriere so lange die nicht zugewiesenen Knoten
					   bis ein Prozess keine Lösung findet und die nicht zugewiesenen Knoten weitervearbeiten kann.
				- Breitensuche könnte unter Umständen viel dynamischen Speicher verbrauchen
			
			Optimierungen:
			
		
		


Referenzen:
[1]https://github.com/vduan/parallel-sudoku-solver
[2]https://www.geeksforgeeks.org/sudoku-backtracking-7/
[3]https://slideshare.net/FernandoJunior52/bdfsdfspaper
