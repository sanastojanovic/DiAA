class NetworkGraph:

    def __init__(self):
        self.graph = {}

    def add_router(self, router):
        if router not in self.graph:
            self.graph[router] = []

    def add_connection(self, u, v, delay, capacity, load):

        edge_uv = {
            "node": v,
            "delay": delay,
            "capacity": capacity,
            "load": load
        }

        edge_vu = {
            "node": u,
            "delay": delay,
            "capacity": capacity,
            "load": load
        }

        self.graph[u].append(edge_uv)
        self.graph[v].append(edge_vu)

    def get_edge(self, u, v):

        for edge in self.graph[u]:
            if edge["node"] == v:
                return edge

        return None