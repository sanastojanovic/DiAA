import heapq


def extended_dijkstra(graph, start, end):

    # Najbolja pronađena vrednost za svaki ruter
    distances = {
        router: float("inf")
        for router in graph
    }

    # Prethodni ruter na optimalnom putu
    previous = {
        router: None
        for router in graph
    }

    distances[start] = 0

    # Prioritetni red
    priority_queue = [(0, start)]

    while priority_queue:

        current_distance, current = heapq.heappop(
            priority_queue
        )

        # Ako smo već pronašli bolji put
        if current_distance > distances[current]:
            continue

        # Stigli smo do odredišta
        if current == end:
            break

        # Obilazak susednih rutera
        for edge in graph[current]:

            neighbor = edge["node"]

            delay = edge["delay"]
            load = edge["load"]

            # Opterećenje utiče na težinu veze
            load_factor = 1 + load / 100

            # Težina veze
            weight = delay * load_factor

            # Nova ukupna težina puta
            new_distance = (
                current_distance + weight
            )

            # Ako smo pronašli bolji put
            if new_distance < distances[neighbor]:

                distances[neighbor] = new_distance

                previous[neighbor] = current

                heapq.heappush(
                    priority_queue,
                    (new_distance, neighbor)
                )

    # Ako ne postoji put do odredišta
    if distances[end] == float("inf"):
        return None, None

    # --------------------------------
    # Rekonstrukcija optimalne rute
    # --------------------------------

    path = []

    current = end

    while current is not None:

        path.append(current)

        current = previous[current]

    path.reverse()

    # --------------------------------
    # Računanje ukupnog kašnjenja
    # --------------------------------

    total_delay = 0

    for i in range(len(path) - 1):

        u = path[i]
        v = path[i + 1]

        # Pronalazimo vezu između u i v
        for edge in graph[u]:

            if edge["node"] == v:

                total_delay += edge["delay"]

                break

    return path, total_delay