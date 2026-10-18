import networkx as nx
import matplotlib.pyplot as plt

from PyQt5.QtWidgets import (
    QWidget,
    QVBoxLayout,
    QHBoxLayout,
    QLabel,
    QComboBox,
    QPushButton,
    QSlider,
    QTableWidget,
    QTableWidgetItem,
    QMessageBox
)

from PyQt5.QtCore import Qt

from matplotlib.backends.backend_qt5agg import (
    FigureCanvasQTAgg as FigureCanvas
)

from dijkstra import extended_dijkstra


class NetworkCanvas(FigureCanvas):

    def __init__(self, parent=None):

        self.figure, self.ax = plt.subplots()

        super().__init__(self.figure)

        self.graph = nx.Graph()

        self.pos = {}

        self.highlighted_path = []

    def draw_network(self, network):

        self.ax.clear()

        self.graph.clear()

        # Dodavanje rutera u graf
        for router in network.graph:

            self.graph.add_node(router)

        # Dodavanje veza
        for u in network.graph:

            for edge in network.graph[u]:

                v = edge["node"]

                if not self.graph.has_edge(u, v):

                    self.graph.add_edge(u, v)

        # Određivanje pozicija rutera
        if not self.pos:

            self.pos = nx.spring_layout(
                self.graph,
                seed=10
            )

        # Crtanje rutera
        nx.draw_networkx_nodes(
            self.graph,
            self.pos,
            ax=self.ax,
            node_size=1200
        )

        # Nazivi rutera
        nx.draw_networkx_labels(
            self.graph,
            self.pos,
            ax=self.ax,
            font_size=12
        )

        normal_edges = []
        path_edges = []

        # Razdvajanje običnih veza i
        # veza koje pripadaju optimalnoj ruti
        for u, v in self.graph.edges():

            if self.is_path_edge(u, v):

                path_edges.append((u, v))

            else:

                normal_edges.append((u, v))

        # Crtanje običnih veza
        nx.draw_networkx_edges(
            self.graph,
            self.pos,
            edgelist=normal_edges,
            ax=self.ax,
            width=2
        )

        # Crtanje optimalne rute
        nx.draw_networkx_edges(
            self.graph,
            self.pos,
            edgelist=path_edges,
            ax=self.ax,
            width=5
        )

        self.ax.set_axis_off()

        self.draw()

    def is_path_edge(self, u, v):

        for i in range(
            len(self.highlighted_path) - 1
        ):

            a = self.highlighted_path[i]
            b = self.highlighted_path[i + 1]

            if (u == a and v == b) or \
               (u == b and v == a):

                return True

        return False


class MainWindow(QWidget):

    def __init__(self):

        super().__init__()

        self.setWindowTitle(
            "Rutiranje u računarskoj mreži"
        )

        self.resize(1100, 750)

        self.create_network()

        self.canvas = NetworkCanvas()

        self.create_interface()

        self.update_graph()

    # ---------------------------------
    # Kreiranje računarske mreže
    # ---------------------------------

    def create_network(self):

        from graph import NetworkGraph

        self.network = NetworkGraph()

        # Ruteri
        routers = [
            "R1",
            "R2",
            "R3",
            "R4",
            "R5"
        ]

        for router in routers:

            self.network.add_router(router)

        # Veze:
        #
        # ruter 1
        # ruter 2
        # kašnjenje (ms)
        # kapacitet (Mbps)
        # opterećenje (%)

        self.network.add_connection(
            "R1",
            "R2",
            5,
            100,
            20
        )

        self.network.add_connection(
            "R1",
            "R3",
            8,
            100,
            30
        )

        self.network.add_connection(
            "R2",
            "R4",
            5,
            100,
            25
        )

        self.network.add_connection(
            "R3",
            "R4",
            4,
            100,
            30
        )

        self.network.add_connection(
            "R4",
            "R5",
            6,
            100,
            20
        )

        self.network.add_connection(
            "R3",
            "R5",
            10,
            100,
            40
        )

    # ---------------------------------
    # Kreiranje interfejsa
    # ---------------------------------

    def create_interface(self):

        main_layout = QVBoxLayout()

        # ---------------------------------
        # Izbor početnog i krajnjeg rutera
        # ---------------------------------

        route_layout = QHBoxLayout()

        route_layout.addWidget(
            QLabel("Početni ruter:")
        )

        self.start_combo = QComboBox()

        self.start_combo.addItems(
            self.network.graph.keys()
        )

        route_layout.addWidget(
            self.start_combo
        )

        route_layout.addWidget(
            QLabel("Krajnji ruter:")
        )

        self.end_combo = QComboBox()

        self.end_combo.addItems(
            self.network.graph.keys()
        )

        self.end_combo.setCurrentText("R5")

        route_layout.addWidget(
            self.end_combo
        )

        # Dugme za pronalaženje rute
        self.find_button = QPushButton(
            "Pronađi optimalnu rutu"
        )

        self.find_button.clicked.connect(
            self.find_route
        )

        route_layout.addWidget(
            self.find_button
        )

        main_layout.addLayout(
            route_layout
        )

        # ---------------------------------
        # Graf mreže
        # ---------------------------------

        main_layout.addWidget(
            self.canvas
        )

        # ---------------------------------
        # Tabela veza
        # ---------------------------------

        self.table = QTableWidget()

        self.table.setColumnCount(5)

        self.table.setHorizontalHeaderLabels([
            "Veza",
            "Kašnjenje (ms)",
            "Kapacitet (Mbps)",
            "Opterećenje (%)",
            "Slobodan kapacitet (Mbps)"
        ])

        # Povećavanje širine kolona
        self.table.setColumnWidth(0, 150)
        self.table.setColumnWidth(1, 150)
        self.table.setColumnWidth(2, 170)
        self.table.setColumnWidth(3, 150)
        self.table.setColumnWidth(4, 220)

        main_layout.addWidget(
            self.table
        )

        # ---------------------------------
        # Izbor veze
        # ---------------------------------

        load_layout = QHBoxLayout()

        load_layout.addWidget(
            QLabel("Izaberi vezu:")
        )

        self.edge_combo = QComboBox()

        self.edge_list = []

        for u in self.network.graph:

            for edge in self.network.graph[u]:

                v = edge["node"]

                # Svaku vezu dodajemo samo jednom
                if (v, u) not in self.edge_list:

                    self.edge_list.append(
                        (u, v)
                    )

                    self.edge_combo.addItem(
                        f"{u} - {v}"
                    )

        load_layout.addWidget(
            self.edge_combo
        )

        # ---------------------------------
        # Slider za opterećenje
        # ---------------------------------

        load_layout.addWidget(
            QLabel("Opterećenje:")
        )

        self.load_slider = QSlider(
            Qt.Horizontal
        )

        self.load_slider.setMinimum(0)

        self.load_slider.setMaximum(100)

        self.load_slider.setValue(20)

        self.load_slider.valueChanged.connect(
            self.change_load
        )

        load_layout.addWidget(
            self.load_slider
        )

        # Prikaz procenta opterećenja
        self.load_label = QLabel(
            "20%"
        )

        load_layout.addWidget(
            self.load_label
        )

        main_layout.addLayout(
            load_layout
        )

        # ---------------------------------
        # Rezultat algoritma
        # ---------------------------------

        self.result_label = QLabel(
            "Ruta još nije izračunata."
        )

        main_layout.addWidget(
            self.result_label
        )

        self.setLayout(
            main_layout
        )

    # ---------------------------------
    # Ažuriranje grafa
    # ---------------------------------

    def update_graph(self):

        self.canvas.highlighted_path = []

        self.canvas.draw_network(
            self.network
        )

        self.update_table()

    # ---------------------------------
    # Ažuriranje tabele
    # ---------------------------------

    def update_table(self):

        edges = []

        for u in self.network.graph:

            for edge in self.network.graph[u]:

                v = edge["node"]

                # Provera da li je veza već dodata
                if not any(
                    x[0] == v and x[1] == u
                    for x in edges
                ):

                    edges.append(
                        (u, v, edge)
                    )

        self.table.setRowCount(
            len(edges)
        )

        for row, (u, v, edge) in enumerate(edges):

            # Veza
            self.table.setItem(
                row,
                0,
                QTableWidgetItem(
                    f"{u} - {v}"
                )
            )

            # Kašnjenje
            self.table.setItem(
                row,
                1,
                QTableWidgetItem(
                    str(edge["delay"])
                )
            )

            # Kapacitet
            self.table.setItem(
                row,
                2,
                QTableWidgetItem(
                    str(edge["capacity"])
                )
            )

            # Opterećenje
            self.table.setItem(
                row,
                3,
                QTableWidgetItem(
                    str(edge["load"])
                )
            )

            # Slobodan kapacitet
            free_capacity = (
                edge["capacity"]
                * (1 - edge["load"] / 100)
            )

            self.table.setItem(
                row,
                4,
                QTableWidgetItem(
                    f"{free_capacity:.1f}"
                )
            )

    # ---------------------------------
    # Promena opterećenja
    # ---------------------------------

    def change_load(self, value):

        self.load_label.setText(
            f"{value}%"
        )

        selected = (
            self.edge_combo.currentText()
        )

        u, v = selected.split(" - ")

        # Pronalazimo vezu u oba smera
        edge1 = self.network.get_edge(
            u,
            v
        )

        edge2 = self.network.get_edge(
            v,
            u
        )

        # Postavljamo novo opterećenje
        edge1["load"] = value

        edge2["load"] = value

        # Ažuriranje tabele
        self.update_table()

        # Ponovno računanje optimalne rute
        self.find_route()

    # ---------------------------------
    # Pronalaženje optimalne rute
    # ---------------------------------

    def find_route(self):

        start = self.start_combo.currentText()

        end = self.end_combo.currentText()

        # Početni i krajnji ruter ne mogu biti isti
        if start == end:

            QMessageBox.warning(
                self,
                "Greška",
                "Početni i krajnji ruter moraju biti različiti."
            )

            return

        # Pokretanje proširenog Dijkstra algoritma
        path, total_delay = extended_dijkstra(
            self.network.graph,
            start,
            end
        )

        # Ako put ne postoji
        if path is None:

            self.result_label.setText(
                "Ne postoji put između izabranih rutera."
            )

            return

        # Čuvanje pronađene rute
        self.canvas.highlighted_path = path

        # Ponovno crtanje mreže
        self.canvas.draw_network(
            self.network
        )

        # Prikaz rezultata
        self.result_label.setText(
            "Optimalna ruta: "
            + " → ".join(path)
            + f" | Ukupno kašnjenje: {total_delay} ms"
        )