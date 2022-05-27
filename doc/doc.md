Autoren:
	Martin Klose
	Florian Neufeld
	Konstantin Daitche
	Kamil Jurczenko
Sprecher:
    Kamil Jurczenko 


Dokumentation:
	Ein Sudokufeld wird mithilfe des Backtracking Algorithmus gelöst. Hierfür wird sudoku_seriell.c von "Pradeep Mondal P" verwendet, wobei ein Sudokufeld als 9x9 Matrix betrachtet wird.
	Der Algorithmus verläuft folglich als Pseudocode:
	
	(1) solveSudoku aufrufen
		(2) Abbruchbedingung: 1 Ausgeben falls im letzten Feld 
		(3) Gehe Feld weiter, falls Feld keine Null (Nur im ersten solveSudoku Aufruf relevant)
		(4) Iteriere durch i <= 9 und prüfe ob i in Feld passt (legal nach Sudokuregel ist)
			(4.1) Wenn legal gebe 1 aus und gehe eine Spalte weiter und führe (1) rekursiv aus
			(4.2) Gebe 0 aus falls keine Lösung gefunden
	
	Dieser Algorithmus lässt sich als eine Tiefensuche in einem Graphen vergleichen. Da dieser rekursiv verläuft, wird eine Parallelisierung der Lösungsfindung problematisch.
	Um dieses Problem zu lösen wurden zwei Gruppen gebildet die sich unabhängig voneinander eine Strategie zur Parallelisierung entwickelten.
	
	Strategien:
		(1) Bildung von Teilsudokus für je ein Prozess (siehe Grupp1 Branch im git)
		    Tiefensuche wird zur Breitensuche, was sich parallelisieren lässt
				- Der serielle Code wurde modifiziert
			
			Optimierungen:
			
			Probleme:
				Ein Prozess könnte schnell zu keiner Lösung kommen und es muss ein neues Teilsudoku zugewiesen werden.
		
		


Referenzen:
[1]https://github.com/vduan/parallel-sudoku-solver
[2]https://www.geeksforgeeks.org/sudoku-backtracking-7/

