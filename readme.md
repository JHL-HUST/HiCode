# HICODE
----------

## Compiling

Before compiling, the following software should be installed in your system.

* g++
* Xeltex
* R packages 

Compile both the base algorithms and the HICODE program.  

```sh
$ sh compile.sh
```

After compiling, you should see a binary file 

```sh
$ ./Main
```

## How to Run

Rename the graph file to "graph", and put it into director
```sh
$ ./data/GRAPH_NAME/
```

* The graph file's first line indicating the number of vertex.
* The following lines containing 'i j w' representing an edge between (i,j) with weight w.
* Then write a config file for the graph , named "GRAPH_NAME.config" . 
See configuration details in the next section.

Run HICODE using command:

```sh
$ ./Main GRAPH_NAME.config
```

### Example

```sh
$ ./Main config/synl2.config
you will find the community detection results in
$ ./result_directory/{maxLayer, maxOriginal}*.gen
and the ground truth communities are in
$ ./data/synl2/Layer1.gen
$ ./data/synl2/Layer2.gen
```

## Configuration Details

In config file, each row include a pair of Parameter:Value .
see 'config/synl2.config' as an example.

| Parameter | Description       |
| --------- | -----  |
| DATA_DIR  | The data directory|
| RESULT_DIR | The result directory|
|Ground_Truth | The ground truth (only the partition type should be listed)|
| SingleLayer_Method | The base algorithm of the framework. 'Modularity'/'Infomap'|
| Frameworks | The type of framework , separated by ';' if want multiple. 'Remove'/'Reduce++'|
| Framework_CommunitySizeThres | The threshold for leftover|
| Number_Of_Layers | The choosen Layer Number |
| Number_Of_Iteration | The choosen Iteration Number |
| WeightedGraph | TRUE/FALSE indicating if the weighted graph should be used |
  
#### TIPS
The weakening method 'ReduceWeight' in paper indicates the 'Frameworks' should be 'Reduce++' and the 'WeightedGraph' should be TRUE.

## Code Structure

### Framework
The current framework is implemented in 'Framework_Reduce.h' etc., and the base class is 'Framework.h'. The most important function that need to be implemented is 'calcNextLayerGraph(Graph cur,Community comm)', which gives the current graph and a set of community, return value is the reduced graph.

### Base Algorithms
The current framework is implemented in 'SingleLayer_Modularity.h' etc., and the base class is 'SingleLayer_Method.h'. The most important function that need to be implemented is 'generateCommunity(string graphFile,string communityFile)', which takes in the graph file and using the algorithm to generate the communities and save in communityFile. 

## Announcements
### Licence
Based on the article "Hidden Community Detection in Social Networks"
Copyright (C) 2017 He. Kun, Li. Yingru, Soundarajan. Sucheta, Hopcroft. John E.


This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

### Notification
Please email to us or setup a issue if you have any problems or find any bugs.

Please cite our papers if you use the code in your paper:

            @ARTICLE{2017arXiv170207462H,
              author = {{He}, K. and {Li}, Y. and {Soundarajan}, S. and {Hopcroft}, J.~E.},
              title = "{Hidden Community Detection in Social Networks}",
              journal = {ArXiv e-prints},
              archivePrefix = "arXiv",
              eprint = {1702.07462},
              keywords = {Computer Science - Social and Information Networks, Physics - 
              Physics and Society, Statistics - Machine Learning},
              year = 2017,
              month = feb,
              adsurl = {http://adsabs.harvard.edu/abs/2017arXiv170207462H},
              adsnote = {Provided by the SAO/NASA Astrophysics Data System}
            }

           @article{DBLP:journals/corr/HeSCHH15,
              author    = {Kun He and
              Sucheta Soundarajan and
              Xuezhi Cao and
              John E. Hopcroft and
              Menglong Huang},
              title     = {Revealing Multiple Layers of Hidden Community Structure in Networks},
              journal   = {CoRR},
              volume    = {abs/1501.05700},
              year      = {2015},
              url       = {http://arxiv.org/abs/1501.05700},
              timestamp = {Mon, 02 Feb 2015 14:12:25 +0100},
              biburl    = {http://dblp.uni-trier.de/rec/bib/journals/corr/HeSCHH15},
              bibsource = {dblp computer science bibliography, http://dblp.org}
            }

### Acknowledgement
In the program, we incorporates some open source codes as baseline algorithms from the following websites:

Link communities
https://github.com/bagrow/linkcomm

OSLOM2 
http://www.oslom.org/software.htm

Modularity
https://sourceforge.net/p/louvain/code/ci/default/tree/

Infomap
http://www.mapequation.org/code.html
