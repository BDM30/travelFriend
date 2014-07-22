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


def record_polygons(relation_object, file, objects):
    count = 0
    way_list = []

    for element in relation_object:
        if element.tag == 'member' and element.attrib['type'] == 'way':
            way = find_by_id(objects['ways'], element.attrib['ref'])
            way_list.append(way)

    while len(way_list):
        way = way_list.pop().findall('nd')
        try:
            polygon = get_polygon(way, way_list, objects)
            file.write('var coord_polygon_' + str(count) + ' = [\n')

            for element in polygon:
                file.write('    new google.maps.LatLng' + str(element) + ',\n')

            file.write('];\n\n\n')
            file.write('var polygon_' + str(count) + ' = new google.maps.Polygon({\n')
            file.write('    paths : coord_polygon_' + str(count) + ',\n')
            file.write('    fillColor : ' + '"red"' + ',\n')
            file.write('    map : map\n});\n\n')
            count += 1
        except IncorrectLoop as error:
            print('incorrect_loop error : ', error.msg_error)


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
        coord_polygon.append((node.attrib['lat'], node.attrib['lon']))

    return coord_polygon


def create_html(address):
    tree = ET.parse(address)
    objects = get_objects(tree)
    file = open('test.html', 'w')
    file.write('<html>\n  <head>\n    <script src="http://maps.google.com/maps/api/js?sensor=false"></script>\n')
    file.write('    <script src="script.js"></script>\n    <style> html, body { height: 100%; } </style>\n  ')
    file.write('    <script>\n')
    file.write('window.onload = function() {\n  var options = {\n    zoom : 8,\n    center : new google.maps.LatLng(')
    file.write('59.79769215914156, 30.21131438455466),\n    mapTypeId: google.maps.MapTypeId.TERRAIN\n  };\n')
    file.write('  var map = new google.maps.Map(document.getElementById("google_map"), options);\n\n')

    for element in objects['relations']:
        record_polygons(element, file, objects)

    file.write('\n};\n    </script>\n  </head>\n')
    file.write('  <body>\n    <div id="google_map" style="width:100%; height:100%"></div>\n  </body>\n  </html>\n\n')


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


while True:
    try:
        address = input()
        create_html(address)
        break
    except FileNotFoundError:
        print("File not found, try again")
    except:
        print("Critical error. Program is finishing")
        break

