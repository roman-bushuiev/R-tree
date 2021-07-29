# R-tree

Implementation of the <a href="https://en.wikipedia.org/wiki/R-tree" title="R-tree">R-tree</a> - data structure appropriate for indexing multi-dimensional data in form of hyperrectangles. This project additionally provides a simple GUI allowing to transform the structure, save it and analyze results of the queries:

<img src="doc/imgs/rt-gui1.png" width="40%">
<img src="doc/imgs/rt-gui2.png" width="70%">

### Experiments prove the efficiency of the R-tree

- bounding box query
<p style="text-align:center;">
  <img src="doc/imgs/rt-results1.png" width="40%">
  <img src="doc/imgs/rt-results2.png" width="40%">
</p>

- k-NN query
<p style="text-align:center;">
  <img src="doc/imgs/rt-results3.png" width="40%">
  <img src="doc/imgs/rt-results4.png" width="40%">
</p>
