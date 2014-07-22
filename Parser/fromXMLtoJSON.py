import urllib.request
import xml.etree.ElementTree as ET


class IncorrectLoop(Exception):
    def __init__(self, msg_error):
        self.msg_error = msg_error


def get_objects(tree):
    root = tree.getroot()

    mas_relations = []
    mas_ways = []
    mas_nodes = []

    for element in root:
        if element.tag == 'relation':
            mas_relations.append(element)
        elif element.tag == 'way':
            mas_ways.append(element)
        elif element.tag == 'node':
            mas_nodes.append(element)

    mas_ways.sort(key=sort_by_id)
    mas_nodes.sort(key=sort_by_id)

    return {"relations": mas_relations, "ways": mas_ways, "nodes": mas_nodes}


def sort_by_id(element):
    return element.attrib['id']


def record_polygons(relation_object, objects):
    count = 0
    way_list = []
    global jsonStr

    for element in relation_object:
        if element.tag == 'member' and element.attrib['type'] == 'way':
            way = find_by_id(objects['ways'], element.attrib['ref'])
            way_list.append(way)

    while len(way_list):
        way = way_list.pop().findall('nd')
        try:
            polygon = get_polygon(way, way_list, objects)
            feature = { "type": "Features",
                        "properties": get_property(relation_object),
                        "geometry": {"type": "LineString", "coordinates": []}}

            for coord in polygon:
                feature["geometry"]["coordinates"].append(coord)
            jsonStr["features"].append(feature)

        except IncorrectLoop as error:
            print('incorrect_loop error : ', error.msg_error)


def get_property(relation_object):
    tags = relation_object.findall("tag")
    property = {}

    for tag in tags:
        if tag.attrib["k"] == "admin_level":
            property["level"] = tag.attrib["v"]
        elif tag.attrib["k"] == "name":
            property["name"] = tag.attrib["v"]

    return property


def get_polygon(way, way_list, objects):
    polygon = way
    scope_end = way[len(way) - 1].attrib

    while polygon[0].attrib != polygon[len(polygon) - 1].attrib:
        temp = scope_end
        for next_way in way_list:
            nodes = next_way.findall('nd')
            begin = nodes[0].attrib
            end = nodes[len(nodes) - 1].attrib

            if scope_end == begin or scope_end == end:
                if scope_end == end:
                    scope_end = begin
                    nodes.reverse()
                else:
                    scope_end = end
                polygon += nodes[1:]
                way_list.remove(next_way)
                break
        if temp == scope_end:
            raise IncorrectLoop('Incorrect input. Cannot find polygon. This loop was ignored')

    return get_coord_polygon(polygon, objects)


def get_coord_polygon(polygon, objects):
    coord_polygon = []

    for element in polygon:
        node = find_by_id(objects['nodes'], element.attrib['ref'])
        coord_polygon.append([float(node.attrib['lat']), float(node.attrib['lon'])])

    return coord_polygon


def create_html(address):
    global jsonStr
    jsonStr = { "type": "FeatureCollection",
                "generator": "JSOM",
                "features": []}

    tree = ET.parse(address)
    objects = get_objects(tree)
    file = open('test.html', 'w')

    for element in objects['relations']:
        record_polygons(element, objects)

    file.write(str(jsonStr))


def find_by_id(mas, id):
    begin = 0
    end = len(mas) - 1

    while begin <= end:
        middle = int((begin + end) / 2)
        if mas[middle].attrib['id'] == id:
            return mas[middle]
        elif id > mas[middle].attrib['id']:
            begin = middle + 1
        elif id < mas[middle].attrib['id']:
            end = middle - 1
    return -1

jsonStr = {}

while True:
    try:
        address = input()
        create_html(address)
        break
    except FileNotFoundError:
        print("File not found, try again")
    #except:
        #print("Critical error. Program is finishing")
        #break

