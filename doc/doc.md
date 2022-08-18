Autoren:
	Martin Klose
	Florian Neufeld
	Konstantin Daitche
	Kamil Jurczenko
Sprecher:
    Kamil Jurczenko 


Dokumentation:
	Ein Sudokufeld wird mithilfe des Backtracking Algorithmus (äquivalent zu einer Tiefensuche eines Graphens) gelöst. Hierfür wird sudoku_seriell.c von "Pradeep Mondal P" verwendet [2], wobei ein Sudokufeld als nxn Matrix betrachtet wird (n ist die Größe des Sudokus).
	Der Algorithmus verläuft folglich als Pseudocode:
	
	(1) solveSudoku aufrufen
		(2) Abbruchbedingung: 1 Ausgeben falls im letzten Feld (von links nach rechts und oben nach unten)
		(3) Iteriere solange durch das Sudokufeld bis ein leeres Feld gefunden wurde (ein Feld = 0 ist)
		(4) Iteriere solange durch i <= n bis i in das Feld passt (legal nach Sudokuregeln ist)
			(4.1) Wenn eine Zahl gefunden wurde die reinpasst dann gebe 1 aus und gehe ein Feld weiter und führe (1) rekursiv aus
			(4.2) Gebe 0 aus falls keine Lösung gefunden und gehe zurück zu (4) und probiere die nächsthöhere Zahl aus (Backtracking)
	
	Dieser Algorithmus hat im schlimmsten Fall eine exponentielle Laufzeit von z^N und ist dementsprechend sehr Aufwendig.
	Dieses Problem lässt sich unter Umständen parallelisieren. Da der Algorithmus rekursiv verläuft, wird eine Parallelisierung der Lösungsfindung problematisch, weshalb alternative Lösungen gefunden werden müssen.
	Es wurde folgende Strategie entwickelt:
		
		Man Stelle sich eine Sudoku und alle möglichen Einträge in den Feldern als einen Graphen vor, wobei ein Knoten ein Sudoku ist. 
		Die Wurzel ist das Sudoku was gelöst werden soll und die Kinderknoten weitere Sudokus die aus dem Wurzelsudoku stammen und ein gefülltes Feld besitzen.
	
	Für die Parallelisierung wurden zum Seriellen Code bestimmte Modifikationen durchgeführt:
		
[Serieller Code]		(1) Bildung von Startsudokus, welches ein Prozess abarbeitet 
								Vor der eigentlichen Parallelisierung muss die Methode initParallel(int processCount) im seriellen Code aufgerufen werden.
								initParallel erstellt ausgehend vom Wurzelsudoku Sudokus, indem eine Breitensuche durchgeführt wird. Es wird solange eine Breitensuche durchgeführt bis auf einer Tiefe/Ebene im Graphen mindestens so viele 
								Knoten wie Prozesse sind. Die Sudokus werden in eine Liste abgespeichert.
								
[Parallelisierungsvarianten]	Es wurden zwei Varianten zu der Parallelisierung angewendet:
								(1) Variante 1
									- Prozess 1 erstellt die Sudokus, während alle restlichen Prozesse warten. Nach der Erstellung gibt Prozess 1 jedem verfügbaren Prozess ein Sudoku, mit dem dann eine Lösung gesucht wird.
									  Findet ein Prozess keine Lösung dann sendet dieser eine Nachricht an Prozess 1 und fragt nach einem neuen Sudoku, falls vorhanden.
									  Der Ablauf wird solange wiederholt bis keine Sudokus mehr vorhanden sind, oder irgendwo schon eine Lösung gefunden wurde.
								(2) Variante 2
									- Jeder Prozess ruft initParallel und es entstehen Redundanzen. 
									  Es kann jedoch jeder Prozess ein Sudoku abbarbeiten, wohingegen bei Variante 1 ein Prozess zum Managment verwendet wird der nie Sudokus abbarbeitet.
									  
				
			Mögliche Probleme: 
				- Es werden eine begrenzte Anzahl von Prozessen benötigt, weil, vor allem in weniger komplexen Sudokus, beispielsweise nur maximal 50 Knoten in einer Tiefe im Graphen vorkommen können und die komplette Leistung des Clusters nicht ausgenutzt werden kann.
			Probleme:
				- Der Speicher wird bei sehr großen Sudokus und sehr vielen Prozessen sehr groß (100x100x4Bytes bei int sind 40 MB, bei 1000 Sudokus sind das 40GB benötigter Speicher)
			
			
		
		


Referenzen:
[1]https://github.com/vduan/parallel-sudoku-solver
[2]https://www.geeksforgeeks.org/sudoku-backtracking-7/
