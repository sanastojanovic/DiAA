
# RectilinearMST

## Compilation

`g++ src/*.cpp -o program`

## Execution

`./program <input_type> [--visualize (true | false) [--visualization_filename <filename>]] [--method (naive | prim | kruskal)]` 

where `input_type` :=
		
		random <no. of points> <width> <height> [--seed <seed_number>] [--dispersion <dispersion value>]

		| stdin

		| file <filename>

`random` means the points are generated randomly, `stdin` means they are read from `stdin`, `file` means they are read from a file.

Including `--visualize true` as an argument enables the visualization of all points, as well the MST edges between them, and saves it as the filename determined by the `--visualization_filename` option. `visualization.png`. The visualization can reliably handle values of $N$ up to $100000$, $W$ and $H$ up to $8000$.

For random input generation: 

- `--seed` determines the seed used to generate the points.

- The method of constructing the MST is selected via the --method option. The available arguments are `naive`, `prim`, and `kruskal`. `naive` uses the quadratic naive algorithm to find the MST in the complete graph. `prim` and 'kruskal' first construct a reduced set of edges, then apply Prim's or Kruskal's MST algorithm, respectively. If no method is selected, the program defaults to `prim`.

- `--dispersion` determines the minimum distance between any two generated points. Its main purpose is to ensure nodes don't collide with each other in the visualization. A non-zero `dispersion` value makes the time complexity of point generation quadratic, as the generator checks for collision between all pairs of points, so it is unsuited for generation of larger inputs. For larger random inputs, a `dispersion` value of $0$ should be used, as it omits collision checks, making point generation linear in complexity. Defaults to $0$. 

For example, `./program random 10000 2000 1000 --seed 5 --dispersion 10 --visualize true --visualization_filename picture.png --method kruskal` generates 10000 random points on a 2000x1000 canvas using seed 5, such that any two points are at least 10 units of distance away from each other. After that, a reduced set of edges is constructed, and Kruskal's algorithm is run to generate the MST. Finally, the MST is visualized in a 2000x1000 image named `picture.png`.

Stdin and file input have the following format:

$$N \ W \ H$$

$$X_1 \ Y_1$$

$$X_2 \ Y_2$$

$$\dots $$

$$X_N \ Y_N$$

where $N$ = number of points, $W$ = width, $H$ = height. Point $x$-coordinates range from $0$ to $W$, $y$-coordinates range from $0$ to $H$.

