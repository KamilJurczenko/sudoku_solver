### Autoren:
Florian Neufeld
Konstantin Daitche
Kamil Jurczenko
### Sprecher:
Kamil Jurczenko 


## Dokumentation:
Ein Sudokufeld wird mithilfe des Backtracking Algorithmus (äquivalent zu einer Tiefensuche eines Graphens) gelöst. Hierfür wird sudoku_seriell.c von "Pradeep Mondal P" verwendet [1], wobei ein Sudokufeld als nxn Matrix betrachtet wird (n ist die Größe des Sudokus).

Der Algorithmus verläuft folglich als Pseudocode:
	
	(1) solveSudoku aufrufen
		(2) Abbruchbedingung: 1 Ausgeben falls im letzten Feld <=> Lösung gefunden
		(3) Iteriere solange durch das Sudokufeld bis ein leeres Feld gefunden wurde (ein Feld = 0 ist)
		(4) Iteriere solange durch i <= n bis i in das Feld passt (legal nach Sudokuregeln ist)
			(4.1) Wenn eine Zahl gefunden wurde dann gehe ein Feld weiter und führe (1) rekursiv aus
			(4.2) Gebe 0 aus falls keine Zahl gefunden wurde und gehe ein Feld zurück und probiere die nächsthöhere Zahl aus (Backtracking)
	
Dieser Algorithmus hat im schlimmsten Fall eine exponentielle Laufzeit von z^N und ist dementsprechend sehr Aufwendig.
Dieses Problem lässt sich unter Umständen parallelisieren. Da der Algorithmus rekursiv verläuft, wird eine Parallelisierung der Lösungsfindung problematisch, weshalb alternative Lösungen gefunden werden müssen.
		
Man Stelle sich eine Sudoku und alle möglichen Einträge in den Feldern als einen Graphen vor, wobei ein Knoten ein Sudoku sei. Die Wurzel ist das Sudoku was gelöst werden soll und die Kinderknoten weitere Sudokus die aus dem Wurzelsudoku stammen und ein weiteres gefülltes Feld besitzen. Um die Traversierung des Baumes zu parallelisieren muss der gesamte Baum in einzelne Bäum/Äste geteilt werden, damit ein Prozess diesen abbarbeiten kann. Das Problem ist jedoch die Bereitstellung dieser Äste worauf einige Strategien entwickelt wurden.

TODO 			HIER GRAFIK (BAUM/GRAPH)	
	
Für die Parallelisierung wurden zum Seriellen Code bestimmte Modifikationen durchgeführt:
		
[Serieller Code]		(1) Bildung von Startsudokus, welches ein Prozess abarbeitet 
								Vor der eigentlichen Parallelisierung muss die Methode initParallel(int processCount) im seriellen Code aufgerufen werden.
								initParallel erstellt ausgehend vom Wurzelsudoku Sudokus, indem eine Breitensuche durchgeführt wird. Es wird solange eine Breitensuche durchgeführt bis auf einer Tiefe/Ebene im Graphen mindestens so viele 
								Knoten wie Prozesse sind. Die Sudokus werden in eine Liste abgespeichert.

				
### 	Parallelisierungsvarianten			
Es wurden zwei Varianten zu der Parallelisierung angewendet:
##### (1) Variante mit MPI
- Prozess 1 erstellt die Sudokus, während alle restlichen Prozesse warten. Nach der Erstellung gibt Prozess 1 jedem verfügbaren Prozess ein Sudoku, mit dem dann eine Lösung gesucht wird.
 Findet ein Prozess keine Lösung dann sendet dieser eine Nachricht an Prozess 1 und fragt nach einem neuen Sudoku, falls vorhanden.
 Der Ablauf wird solange wiederholt bis keine Sudokus mehr vorhanden sind, oder irgendwo schon eine Lösung gefunden wurde.

##### (2) Naive Variante mit OMP
- Jeder Prozess ruft initParallel und es entstehen Redundanzen. 
Es kann jedoch jeder Prozess ein Sudoku abbarbeiten, wohingegen bei Variante 1 ein Prozess zum Managment verwendet wird der nie Sudokus abbarbeitet.

##### (3) OMP mit rekursiven Tasks
- Der Master Thread führt auf die Berechnungsmethode eine Task aus. In jedem Task werden weitere Tasks ausgeführt, falls weitere Knoten folgen.
    - Hierbei werden nur so viele Tasks erstellt wie Anzahl Threads um overhead zu vermeiden.


## Messungen

##### (1) Variante mit MPI
GRAFIK			  
##### (2) Naive Variante mit OMP
GRAFIK
##### (3) OMP mit rekursiven Tasks
GRAFIK					  
			
## Fazit
Die Zeiten für die Berechnung eines Sudokus sind bei MPI und OMP sehr ähnlich. Es zeigte sich das die Implementation von OMP einfacher ist. OMP wurde mit 100 Zeilen Code realisiert, wohingegen der MPI Code das doppelte mit ca. 200 Zeilen enthält.

Idealerweise hat man bei 2 Prozessen und Cores eine halbierung der Berechnungszeit als bei nur einem Prozess und Core. Die Messungen zeigten aber das man bei einem 16x16 Sudoku mit Normaler Schwierigkeit mit dem Parallelisierungsverfahren bei 16 Prozessen
und Cores nur eine sehr geringe Verbesserung von ca 0.7 Sekunden verglichen mit nur einem Prozess aufweist (siehe rawdata\old data\mpi\sudokuNormal16). 
			
## Probleme:
- Es werden eine begrenzte Anzahl von Prozessen benötigt, weil, vor allem in weniger komplexen Sudokus, beispielsweise nur maximal 50 Knoten in einer Tiefe im Graphen vorkommen können und die komplette Leistung des Clusters nicht ausgenutzt werden kann. 
- Zu Variante (2) Einige Prozesse finden schnell keine Lösung und müssen warten wenn keine Teilsudokus mehr übrig sind, während andere Prozesse lange rechnen.


Referenzen:
[1]https://www.geeksforgeeks.org/sudoku-backtracking-7/
