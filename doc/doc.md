Autoren:
	Martin Klose
	Florian Neufeld
	Konstantin Daitche
	Kamil Jurczenko
Sprecher:
    Kamil Jurczenko 


Dokumentation:
	Ein Sudokufeld wird mithilfe des Backtracking Algorithmus (äquivalent zu einer Tiefensuche eines Graphens) gelöst. Hierfür wird sudoku_seriell.c von "Pradeep Mondal P" verwendet [2], wobei ein Sudokufeld als nxn Matrix betrachtet wird.
	Der Algorithmus verläuft folglich als Pseudocode:
	
	(1) solveSudoku aufrufen
		(2) Abbruchbedingung: 1 Ausgeben falls im letzten Feld 
		(3) Gehe Feld weiter, falls Feld keine Null (Nur im ersten solveSudoku Aufruf relevant)
		(4) Iteriere durch i <= n (n:=Breite/Höhe des Sudokufeldes) und prüfe ob i in Feld passt (legal nach Sudokuregel ist)
			(4.1) Wenn legal gebe 1 aus und gehe ein Feld weiter und führe (1) rekursiv aus
			(4.2) Gebe 0 aus falls keine Lösung gefunden und gehe zurück zu (4) und probiere eine höhere Zahl
	
	Dieser Algorithmus hat im schlimmsten Fall eine exponentielle Laufzeit von z^N und ist dementsprechend sehr Aufwendig.
	Dieses Problem lässt sich unter Umständen parallelisieren. Da der Algorithmus rekursiv verläuft, wird eine Parallelisierung der Lösungsfindung problematisch, weshalb alternative Lösungen gefunden werden müssen.
	Es wurde folgende Strategie entwickelt:
		
		Man Stelle sich eine Sudoku und alle möglichen Einträge in Felder als einen Graphen vor, wobei ein Knoten ein Sudoku ist. 
		Ein	Kinderknoten ist ein weiteres Sudoku welches ein weiteres, nach den Sudoku Regel entsprechend, gefülltes Feld vom Elternknoten besitzt.
		
[Serieller Code]		(1) Bildung von Startsudokus für je ein Prozess 
						- Der serielle Code wurde dafür modifiziert
							- Vor der eigentlichen Parallelisierung muss die Methode initParallel(int processCount) im seriellen Code aufgerufen werden.
								- initParallel erstellt Sudokus indem eine Breitensuche ausgehend vom gegebenen Sudoku durchgeführt wird.
									- Dafür wurde der gegebene Backtracking Algorithmus ausgenutzt, der sich das nächste Sudoku merkt und in eine Liste abspeichert
										- Es werden solange Sudokus gesucht bis die Anzahl der Sudokus in einer Tiefe/Ebene im Graphen mindestens gleich der Prozessmenge ist und die Liste wird ausgegeben.
								
[Parallelisierungsvarianten]	Es wurden zwei Varianten zu der Parallelisierung geschrieben:
								(1) Variante 1
									- Prozess 1 erstellt die Sudokus, während alle restlichen Prozesse warten. Nach der Erstellung gibt Prozess 1 jedem verfügbaren Prozess ein Sudoku, mit dem dann eine Lösung gesucht wird.
									  Findet ein Prozess keine Lösung dann sendet dieser eine Nachricht an Prozess 1 und fragt nach einem neuen Sudoku, falls vorhanden.
									  Durchgang wird solange wiederholt bis es keine Sudokus mehr von Prozess 1 zum lösen gibt, oder irgendwo schon eine Lösung gefunden wurde.
								(2) Variante 2
									- Jeder Prozess ruft initParallel und es entstehen Redundanzen. 
									  Es kann jedoch jeder Prozess ein Sudoku abbarbeiten, wohingegen Variante 1 ein Prozess nie Sudokus abbarbeiten wird.
									  
				
			Mögliche Probleme: 
				- Es werden eine maximale Anzahl von Prozessen benötigt, weil beispielsweise in einem Sudoku nur maximal 50 Knoten in einer Tiefe im Graphen vorkommen können.
			Probleme:
				- Der Speicher wird bei sehr großen Sudokus und sehr vielen Prozessen sehr groß (100x100x4Bytes bei int sind 40 MB, bei 1000 Sudokus sind das 40GB benötigter Speicher)
			
			Optimierungen:
				- Frühzeitiges Abbrechen bei Lösung
		
		


Referenzen:
[1]https://github.com/vduan/parallel-sudoku-solver
[2]https://www.geeksforgeeks.org/sudoku-backtracking-7/
