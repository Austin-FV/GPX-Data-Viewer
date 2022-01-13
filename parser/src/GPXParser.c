/**
 * section: Tree
 * synopsis: Navigates a tree to print element names
 * purpose: Parse a file to a tree, use xmlDocGetRootElement() to
 *          get the root element, then walk the document and print
 *          all the element name in document order.
 * usage: tree1 filename_or_URL
 * test: tree1 test2.xml > tree1.tmp && diff tree1.tmp $(srcdir)/tree1.res
 * author: Dodji Seketeli
 * copy: see Copyright for the status of this software.
 */

#include "GPXParser.h"


/*
 *To compile this file using gcc you can type.  Alternatively, see the Makefile for include path settings
 *gcc `xml2-config --cflags --libs` -o xmlexample2 GPXParser.c LinkedListAPI.c -lm
 */

Waypoint* readWaypoint(xmlNode* w_node){

    Waypoint* waypoint = malloc(sizeof(Waypoint));

    waypoint->name = (char*)malloc(1);
    strcpy(waypoint->name,"\0"); //must initialize as "" later if no name CHANGE THIS
    waypoint->longitude = 0.0;
    waypoint->latitude = 0.0;
    waypoint->otherData = initializeList(&gpxDataToString,&deleteGpxData,&compareGpxData);

    xmlNode* cur_node = NULL;
    for (cur_node = w_node->children; cur_node != NULL; cur_node = cur_node->next){

        if (strcmp((char*)cur_node->name, "name") == 0){

            free(waypoint->name);
            waypoint->name = (char*) malloc(sizeof(char) * strlen((char *)cur_node->children->content) + 1);
            strcpy(waypoint->name,(char*)cur_node->children->content);

        } else if (cur_node->children != NULL) {
            if ((char*)cur_node->name == NULL){
                cur_node->name = (xmlChar*)malloc(1);
                strcpy((char*)cur_node->name,"\0");
            }
            if ((char*)cur_node->children->content == NULL){
                cur_node->children->content = (xmlChar*)malloc(1);
                strcpy((char*)cur_node->children->content,"\0");
            }

            char* cont = (char *)xmlNodeGetContent(cur_node->children);

            GPXData* waypointData = (GPXData *)malloc(sizeof(GPXData) * strlen((char*)cur_node->children->content) + 1);

            strcpy(waypointData->name, (char *) cur_node->name);
            strcpy(waypointData->value, cont);
            free(cont);
            

            insertBack(waypoint->otherData, waypointData);

        }
    }

    xmlAttr *attr = NULL;
    for (attr = w_node->properties; attr != NULL; attr = attr->next){
        if (strcmp((char*)attr -> name, "lon") == 0){
           
            waypoint -> longitude = atof((char*)attr -> children -> content);
        }
        else if (strcmp((char*)attr -> name, "lat") == 0){
            
            waypoint -> latitude = atof((char*)attr -> children -> content);
        }
    }

    return waypoint;
}

Route* readRoute(xmlNode* r_node){

    Route* route = (Route*)malloc(sizeof(Route));

    //route->name = "";

    route->name = (char *)malloc(1);
    strcpy(route->name,"\0"); //must initialize as "" later if no name CHANGE THIS
    route->waypoints = initializeList(&waypointToString,&deleteWaypoint,&compareWaypoints);
    route->otherData = initializeList(&gpxDataToString,&deleteGpxData,&compareGpxData);

    xmlNode* cur_node = NULL;
    for (cur_node = r_node->children; cur_node != NULL; cur_node = cur_node->next){

        if (strcmp((char*)cur_node->name, "name") == 0){
            
            free(route->name);

            route->name = (char*) malloc(sizeof(char) * strlen((char*)cur_node->children->content) + 1);
            strcpy(route->name,(char*)cur_node->children->content);

        } else if(strcmp((char*)cur_node->name, "rtept") == 0) {
            Waypoint* waypoint = readWaypoint(cur_node);
            insertBack(route->waypoints,waypoint);
        } else if (cur_node->children != NULL) {
            if ((char*)cur_node->name == NULL){
                cur_node->name = (xmlChar*)malloc(1);
                strcpy((char*)cur_node->name,"\0");
            }
            if ((char*)cur_node->children->content == NULL){
                cur_node->children->content = (xmlChar*)malloc(1);
                strcpy((char*)cur_node->children->content,"\0");
            }
            char* cont = (char *)xmlNodeGetContent(cur_node->children);

            GPXData* routeData = malloc(sizeof(GPXData) + strlen((char*)cur_node->children->content)+1);

            
            strcpy(routeData->name, (char *) cur_node->name);
            strcpy(routeData->value, cont);
            free(cont);

            

            insertBack(route->otherData, routeData);

        }
    }

    return route;
}

Track* readTrack(xmlNode* t_node){


    Track* track = (Track*)malloc(sizeof(Track));

    track->name = ""; //must initialize as "" later if no name CHANGE THIS
    track->segments = initializeList(&trackSegmentToString,&deleteTrackSegment,&compareTrackSegments);
    track->otherData = initializeList(&gpxDataToString,&deleteGpxData,&compareGpxData);
    

    xmlNode* cur_node = NULL;
    for (cur_node = t_node->children; cur_node != NULL; cur_node = cur_node->next){
        

        if (strcmp((char*)cur_node->name, "name") == 0){


            track->name = (char*) malloc(sizeof(char) * strlen((char*)cur_node->children->content) + 1);
            strcpy(track->name,(char*)cur_node->children->content);

        } else if(strcmp((char*)cur_node->name, "trkseg") == 0) {

            TrackSegment* segment = (TrackSegment*)malloc(sizeof(TrackSegment));
            segment->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
            xmlNode* c_node = NULL;
            for (c_node = cur_node->children; c_node != NULL; c_node = c_node->next){
                if (strcmp((char*)c_node->name, "trkpt") == 0){
                    Waypoint* waypoint = readWaypoint(c_node);
                    insertBack(segment->waypoints,waypoint);
                }
            }

            insertBack(track->segments,segment);
        } else if (cur_node->children != NULL) {


            if ((char*)cur_node->name == NULL){
                cur_node->name = (xmlChar*)malloc(1);
                strcpy((char*)cur_node->name,"\0");
            }
            if ((char*)cur_node->children->content == NULL){
                cur_node->children->content = (xmlChar*)malloc(1);
                strcpy((char*)cur_node->children->content,"\0");
            }


            GPXData* trackData = (GPXData*)(malloc(sizeof(GPXData) * strlen((char*)cur_node->children->content) * sizeof(char) + 1));

            strcpy(trackData->name, (char *) cur_node->name);
            //waypointData->value = malloc(sizeof(char) * strlen(cur_node->children->content) + 1);
            strcpy(trackData->value, (char*)cur_node->children->content);
            
            //CHECK THIS / CHANGE THIS gpxDataToString not working

            insertBack(track->otherData, trackData);

        }
    }
    return track;
}

GPXdoc* createGPXdoc(char* fileName){

    if (fileName == NULL){
        printf("error: improper filename\n");
        return NULL;
    }
    // if (strcmp(strrchr(fileName, '.'), ".gpx") != 0){
    //     return NULL;
    // }

    GPXdoc *newGPXdoc = (GPXdoc*)malloc(sizeof(GPXdoc));

    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    /*parse the file and get the DOM */
    doc = xmlReadFile(fileName, NULL, 0);

    if (doc == NULL) {
        xmlFreeDoc(doc);
        xmlCleanupParser();

        newGPXdoc->creator = (char *)malloc(1);
        strcpy(newGPXdoc->creator,"\0");

        newGPXdoc->waypoints = initializeList(&waypointToString,&deleteWaypoint,&compareWaypoints);
        newGPXdoc->routes = initializeList(&routeToString,&deleteRoute,&compareRoutes);
        newGPXdoc->tracks = initializeList(&trackToString,&deleteTrack,&compareTracks);

        deleteGPXdoc(newGPXdoc);
        printf("error: could not parse file %s\n", fileName);
        return NULL;
    }

    root_element = xmlDocGetRootElement(doc);
    if (strcmp((char*)root_element->name,"gpx") != 0){
        xmlFreeDoc(doc);
        xmlCleanupParser();
        printf("error: root element must be gpx, not %s\n", root_element->name);
        return NULL;
    }

    xmlNode *a_node = root_element;
    xmlNode *cur_node = NULL;

    //update this //CHANGE CHECK
    char *namespace = (char *)root_element->ns->href;
    // char namespace[256] = "http://www.w3.org/2001/XMLSchema-instance";
    strcpy(newGPXdoc->namespace,namespace);

    //this goes through first attribute to get version and creator
    for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {
        // if (cur_node->type == XML_ELEMENT_NODE) {
        //     // printf("node type: Element, name: %s\n", cur_node->name);
        // }

        // Uncomment the code below if you want to see the content of every node.
        // for stuff like elevation <ele>

        // if (cur_node->content != NULL ){
        //     printf("  content: %s\n", cur_node->content);
        // }

        // Iterate through every attribute of the current node
        xmlAttr *attr;
        for (attr = cur_node->properties; attr != NULL; attr = attr->next)
        {
            xmlNode *value = attr->children;
            char *attrName = (char *)attr->name;
            char *cont = (char *)(value->content);

            
            if (strcmp(attrName, "version") == 0){

                newGPXdoc->version = atof(cont);
            }
            if (strcmp(attrName, "creator") == 0){
                newGPXdoc->creator = (char *)malloc(strlen(cont) * sizeof(char) + 1);
                //can't be empty, therefore fill with "" if no content found
                strcpy(newGPXdoc->creator,cont);
            }

            //printf("\tattribute name: %s, attribute value = %s\n", attrName, cont);
        }       

    }

    List* waypointList = initializeList(&waypointToString,&deleteWaypoint,&compareWaypoints);
    List* routeList = initializeList(&routeToString,&deleteRoute,&compareRoutes);
    List* trackList = initializeList(&trackToString,&deleteTrack,&compareTracks);

    cur_node = NULL;

    for(cur_node = root_element->children; cur_node != NULL; cur_node = cur_node->next){
        if (strcmp((char*)cur_node->name, "wpt") == 0){


            Waypoint* waypoint = readWaypoint(cur_node);

            insertBack(waypointList,waypoint);
        }

        if (strcmp((char*)cur_node->name, "rte") == 0){
            Route* route = readRoute(cur_node);
            insertBack(routeList,route);
        }

        if(strcmp((char*)cur_node->name, "trk") == 0){
            Track* track = readTrack(cur_node);
            insertBack(trackList,track);

        }

        
    }

    xmlFreeDoc(doc);

    xmlCleanupParser();


    newGPXdoc->waypoints = waypointList;
    newGPXdoc->routes = routeList;
    newGPXdoc->tracks = trackList;

    // freeList(waypointList);
    // freeList(routeList);
    // freeList(trackList);

    // char* docStr = GPXdocToString(newGPXdoc);

    // free(docStr);

    return newGPXdoc;

}
//MEMLEAK
char* GPXdocToString(GPXdoc* doc){

    if (doc == NULL){
        return NULL;
    }
    char* docStr;

    char* wypStr = toString(doc->waypoints);
    char* routeStr = toString(doc->routes);
    char* trackStr = toString(doc->tracks); 

    docStr = (char *)malloc(strlen(wypStr) + strlen(routeStr) + strlen(trackStr) + strlen(doc->namespace) + strlen(doc->creator) + sizeof(double) + 5 + 46);

    sprintf(docStr, "namespace = %s\tcreator = %s\tversion = %.1f\n", doc->namespace, doc->creator, doc->version);

    strcat(docStr,wypStr);
    strcat(docStr,routeStr);
    strcat(docStr,trackStr);

    free(wypStr);
    free(routeStr);
    free(trackStr);

    return docStr;
} 

//delete gpx doc, clear lists and free  everything
void deleteGPXdoc(GPXdoc* doc){
    
    if (doc == NULL){
        return;
    }

    //check if null first
    // free(doc->namespace);
    //free((void *)doc->version);
    free(doc->creator);

    freeList(doc->waypoints);
    freeList(doc->routes);
    freeList(doc->tracks);

    free(doc);
}





//GPX Data helper functions
void deleteGpxData( void* data){
    if (data == NULL){
        return;
    }

    GPXData* dataPtr = (GPXData*) data;

    free(dataPtr);

}
char* gpxDataToString( void* data){
    if (data == NULL){
        return NULL;
    }


    // char* dataStr;
    char* str;

    int len; 

    GPXData* dataPtr = (GPXData*) data;

    // char* name = dataPtr->name;
    // char* value = dataPtr->value;
    

    len = (strlen(dataPtr->name) + strlen(dataPtr->value) + 2 + 22);
    str = (char *)malloc(len);
    sprintf(str, "name = %s\n value = %s\n",dataPtr->name,dataPtr->value);


    // printf("data check\n");
    // sprintf(buffer, "name = %s\n value = %s\n",name,value);
    // dataStr = malloc(sizeof(char) * strlen(buffer) + 1);
    // strcpy(dataStr,buffer);
    // printf("data check2\n");
    // printf("%s\n", dataStr);
    return str;
}
int compareGpxData(const void *first, const void *second){
    if (first == NULL && second == NULL){
        return 1;
    } else {
        return 0;
    }

    // GPXData* data1 = (GPXData*) first;
    // GPXData* data2 = (GPXData*) second;

    // if (strcmp(data1->name, data2->name) != 0){
    //     return 0;
    // }

    // if(strcmp(data1->value, data2->value) != 0){
    //     return 0;
    // }

    return 1;


    
}



//waypoint helper functions
void deleteWaypoint(void *data){
    if (data == NULL){
        return;
    }

    Waypoint* wyp = (Waypoint*) data;
    if (wyp->name != NULL){
        free(wyp->name);
    }

    freeList(wyp->otherData);
    free(wyp);
}

char* waypointToString(void* data){

    if (data == NULL){
        return NULL;
    }

    //CHANGE THIS DONT NEED ALL THESE VARIABLES

    Waypoint* wyp = (Waypoint*) data;
    // char* name = wyp -> name;
    // double lon = wyp -> longitude;
    // double lat = wyp -> latitude;
    char* wypStr;

    char* dataStr = toString(wyp->otherData);

    //int len = (strlen());

    wypStr = (char *)malloc(sizeof(char) * (strlen(wyp->name) + sizeof(double)*2 + strlen(dataStr) + 2 + 55));

    //char* buffer = malloc(sizeof(char) * ( strlen(name) + strlen(value) +2 );

    // printf("waypoint check\n");

    sprintf(wypStr, "Waypoint name = %s\nlongitude = %.5f\tlatitude = %.5f\n", wyp->name, wyp->longitude, wyp->latitude);

    // wypStr = malloc(sizeof(char) * strlen(buffer) + 1);
    // strcpy(wypStr,buffer);

    strcat(wypStr, dataStr);
    free(dataStr);

    // printf("waypoint str: %s\n",wypStr);

    return wypStr;
}

int compareWaypoints(const void *first, const void *second){
    if (first == NULL && second == NULL){
        return 1;
    } else {
        return 0;
    }

    // // Waypoint* wyp1 = (Waypoint*) first;
    // Waypoint* wyp2 = (Waypoint*) second;

    // if (strcmp(wyp1->name, wyp2->name) != 0){
    //     return 0;
    // }
    // if (wyp1->longitude != wyp2->longitude){
    //     return 0;
    // }
    // if (wyp1->latitude != wyp2->latitude){
    //     return 0;
    // }

    // List* wyp1Data = wyp1->otherData;
    // List* wyp2Data = wyp2->otherData;
    
    // if (wyp1Data == NULL && wyp2Data == NULL){
    //     return 1;
    // } else {
    //     return 0;
    // }

    // if (wyp1Data->length != wyp2Data->length){
    //     return 0;
    // }

    // ListIterator wyp1DataIter = createIterator(wyp1Data);
    // ListIterator wyp2DataIter = createIterator(wyp2Data);

    // void* list1Data = nextElement(&wyp1DataIter);
    // void* list2Data = nextElement(&wyp2DataIter);
    
    // while(list1Data != NULL && list2Data != NULL){        

    //     if (compareGpxData(list1Data,list2Data) == 0){
    //         return 0;
    //     }
    //     list1Data = nextElement(&wyp1DataIter);
    //     list2Data = nextElement(&wyp2DataIter);
    // }

    // return 1;

    return 0;
}

// Route helper functions

void deleteRoute(void* data){
    if(data == NULL){
        return;
    }

    Route* rPtr = (Route*) data;
    if (rPtr->name != NULL){
        free(rPtr->name);
    }

    freeList(rPtr->waypoints);
    freeList(rPtr->otherData);

    free(rPtr);
}

char* routeToString(void* data){
    if (data == NULL){
        return NULL;
    }

    Route* rPtr = (Route*)data;
    char* routeStr;
    // char buffer[512] = "";

    char* dataStr = toString(rPtr->otherData);
    char* wypStr = toString(rPtr->waypoints);


    routeStr = (char *)malloc(sizeof(char) * (strlen(dataStr) + strlen(wypStr) + strlen(rPtr->name) + 3 + 11));

    sprintf(routeStr,"name = %s\n", rPtr->name);

    strcat(routeStr, wypStr);
    strcat(routeStr, dataStr);

    free(wypStr);
    free(dataStr);

    return routeStr;
}

int compareRoutes(const void *first, const void *second){

    if (first == NULL && second == NULL){
        return 1;
    } else {
        return 0;
    }

    // Route* route1 = (Route*) first;
    // Route* route2 = (Route*) second;

    // if (strcmp(route1->name,route2->name) != 0){
    //     return 0;
    // }

    // List* route1DataList = route1->otherData;
    // List* route2DataList = route2->otherData;
    
    // if (route1DataList == NULL && route2DataList == NULL){
    //     return 1;
    // } else {
    //     return 0;
    // }

    // if (route1DataList->length != route2DataList->length){
    //     return 0;
    // }

    // ListIterator route1DataIter = createIterator(route1DataList);
    // ListIterator route2DataIter = createIterator(route2DataList);

    // void* list1Data = nextElement(&route1DataIter);
    // void* list2Data = nextElement(&route2DataIter);
    
    // while(list1Data != NULL && list2Data != NULL){        

    //     if (compareGpxData(list1Data,list2Data) != 0){
    //         return 0;
    //     }
    //     list1Data = nextElement(&route1DataIter);
    //     list2Data = nextElement(&route2DataIter);
    // }

    // //waypoints list comparison //

    // List* route1WypList = route1->waypoints;
    // List* route2WypList = route2->waypoints;
    
    // if (route1WypList == NULL && route2WypList == NULL){
    //     return 1;
    // } else {
    //     return 0;
    // }

    // if (route1WypList->length != route2WypList->length){
    //     return 0;
    // }

    // ListIterator route1WypIter = createIterator(route1WypList);
    // ListIterator route2WypIter = createIterator(route2WypList);

    // void* list1WypData = nextElement(&route1WypIter);
    // void* list2WypData = nextElement(&route2WypIter);
    
    // while(list1WypData != NULL && list2WypData != NULL){        

    //     if (compareWaypoints(list1WypData,list2WypData) == 0){
    //         return 0;
    //     }
    //     list1WypData = nextElement(&route1WypIter);
    //     list2WypData = nextElement(&route2WypIter);
    // }

    return 1;

}

////TRACK SEGMENT HELPER FUNCTIONS
void deleteTrackSegment(void* data){
    if (data == NULL){
        return;
    }

    TrackSegment* segmentPtr = (TrackSegment*) data;
    freeList(segmentPtr->waypoints);
    free(segmentPtr);
}
char* trackSegmentToString(void* data){
    if (data == NULL){
        return NULL;
    }

    
    TrackSegment* segmentPtr = (TrackSegment*) data;
    char* segmentStr;

    char* wypStr = toString(segmentPtr->waypoints);


    segmentStr = (char *)malloc(sizeof(char) * (strlen(wypStr) + 1 + 17));
    strcpy(segmentStr, "Track Segment: \n");

    strcat(segmentStr, wypStr);

    free(wypStr);

    return segmentStr;
}
int compareTrackSegments(const void *first, const void *second){
    if (first == NULL && second == NULL){
        return 1;
    } else {
        return 0;
    }

    // TrackSegment* segment1 = (TrackSegment*) first;
    // TrackSegment* segment2 = (TrackSegment*) second;

    // List* wypList1 = segment1->waypoints;
    // List* wypList2 = segment2->waypoints;

    // if (wypList1 == NULL && wypList2 == NULL){
    //     return 1;
    // } else {
    //     return 0;
    // }

    // if(wypList1->length != wypList2->length){
    //     return 0;
    // }

    // ListIterator wypList1Iter = createIterator(wypList1);
    // ListIterator wypList2Iter = createIterator(wypList2);

    // void* list1Data = nextElement(&wypList1Iter);
    // void* list2Data = nextElement(&wypList2Iter);

    // while(list1Data != NULL && list2Data != NULL){
    //     if(compareWaypoints(list1Data,list2Data) != 0){
    //         return 0;
    //     }
    //     list1Data = nextElement(&wypList1Iter);
    //     list2Data = nextElement(&wypList2Iter);
    // }

    return 1;
    
}

//track helper functions
void deleteTrack(void* data){
    if(data == NULL){
        return;
    }

    Track* trackPtr = (Track*) data;
    if (trackPtr->name != NULL){
        free(trackPtr->name);
    }

    freeList(trackPtr->otherData);
    freeList(trackPtr->segments);

    free(trackPtr);
}
char* trackToString(void* data){
    if (data == NULL){
        return NULL;
    }

    
    Track* trackPtr = (Track*) data;
    char* trackStr;
    // char buffer[1024] = "";

    char* segmentStr = toString(trackPtr->segments);
    char* dataStr = toString(trackPtr->otherData);

    trackStr = (char *)malloc( sizeof(char) * (strlen(segmentStr) + strlen(dataStr) + strlen(trackPtr->name) + 3 + 17 + 6 + 6));


    sprintf(trackStr, "-----\nTrack name = %s\n", trackPtr->name);

    strcat(trackStr,dataStr);
    strcat(trackStr,segmentStr);
    
    strcat(trackStr,"\n-----");

    free(segmentStr);
    free(dataStr);

    //strcpy(trackStr,buffer);

    return trackStr;
}
int compareTracks(const void *first, const void *second){
    if (first == NULL && second == NULL){
        return 1;
    } else{
        return 0;
    }

    // Track* track1 = (Track*)first;
    // Track* track2 = (Track*)second;

    // if(strcmp(track1->name, track2->name) != 0){
    //     return 0;
    // }

    // List* segmentList1 = track1->segments;
    // List* segmentList2 = track2->segments;

    // if (segmentList1 == NULL && segmentList2 == NULL){
    //     return 1;
    // } else {
    //     return 0;
    // }

    // if(segmentList1->length != segmentList2->length){
    //     return 0;
    // }

    // ListIterator segList1Iter = createIterator(segmentList1);
    // ListIterator segList2Iter = createIterator(segmentList2);
    
    // void* segList1Data = nextElement(&segList1Iter);
    // void* segList2Data = nextElement(&segList2Iter);

    // while (segList1Data != NULL && segList2Data != NULL){
    //     if (compareTrackSegments(segList1Data,segList2Data) != 0){
    //         return 0;
    //     }
    //     segList1Data = nextElement(&segList1Iter);
    //     segList2Data = nextElement(&segList2Iter);
    // }

    // //other data list comparison //CHANGE THIS
    // List* dataList1 = track1->otherData;
    // List* dataList2 = track2->otherData;

    // if (dataList1 == NULL && dataList2 == NULL){
    //     return 1;
    // } else {
    //     return 0;
    // }

    // if(dataList1->length != dataList2->length){
    //     return 0;
    // }

    // ListIterator dataList1Iter = createIterator(dataList1);
    // ListIterator dataList2Iter = createIterator(dataList2);
    
    // void* dataList1Data = nextElement(&dataList1Iter);
    // void* dataList2Data = nextElement(&dataList2Iter);

    // while (dataList1Data != NULL && dataList2Data != NULL){
    //     if (compareTrackSegments(dataList1Data,dataList2Data) != 0){
    //         return 0;
    //     }
    //     dataList1Data = nextElement(&dataList1Iter);
    //     dataList2Data = nextElement(&dataList2Iter);
    // }
    
    return 1;
}


//// A1 MODULE 2

//Total number of waypoints in the GPX file
int getNumWaypoints(const GPXdoc* doc){
    // printf("check\n");
    if (doc == NULL || doc->waypoints == NULL){
        return 0;
    }

    //length taken from api
    int num = 0;;
    
    // printf("check\n");
    
    ListIterator iter = createIterator(doc->waypoints);
    void* elem;// = nextElement(&iter);
    while ((elem = nextElement(&iter)) != NULL){
        // Waypoint* waypoint = (Waypoint*)elem;
        // printf("check\n");
        num++;
    }
    

    // printf("%d\n", doc->waypoints->length);
    // return doc->waypoints->length;

    return num;
}

//Total number of routes in the GPX file
int getNumRoutes(const GPXdoc* doc){
    if (doc == NULL || doc->routes == NULL){
        return 0;
    }

    return doc->routes->length;
}

//Total number of tracks in the GPX file
int getNumTracks(const GPXdoc* doc){
    if (doc == NULL || doc->tracks == NULL){
        return 0;
    }

    return doc->tracks->length;
}

//Total number of segments in all tracks in the document
int getNumSegments(const GPXdoc* doc){
    if (doc == NULL || doc->tracks == NULL){
        return 0;
    }

    int numSeg = 0;
    ListIterator listIter = createIterator(doc->tracks);

    void* listElement = nextElement(&listIter);

    while(listElement != NULL){
        Track* track = (Track*) listElement;

        if(track->segments != NULL){
            numSeg += track->segments->length;
            
        }
        listElement = nextElement(&listIter);
    }

    return numSeg;

}

//Total number of GPXData elements in the document
int getNumGPXData(const GPXdoc* doc){
    if (doc == NULL){
        return 0;
    }

    int numData = 0;


    ListIterator wptIter = createIterator(doc->waypoints);
    ListIterator routeIter = createIterator(doc->routes);
    ListIterator trackIter = createIterator(doc->tracks);

    void* wptElement = nextElement(&wptIter);
    void* routeElement = nextElement(&routeIter);
    void* trackElement = nextElement(&trackIter);

    while (wptElement != NULL){
        
        Waypoint* waypoint = (Waypoint*) wptElement;
        if (waypoint->name != NULL && strlen(waypoint->name) != 0){

            numData++;
        }
        if(waypoint->otherData != NULL){
            numData+=(waypoint->otherData->length);
        }
        wptElement = nextElement(&wptIter);
    }

    while (routeElement != NULL){

        Route* route = (Route*) routeElement;
        if (route->name != NULL && strlen(route->name) != 0){
            numData++;
        }
        if (route->otherData != NULL) {
            numData+=(route->otherData->length);
        }

        ListIterator wpt2Iter = createIterator(route->waypoints);
        void* wpt2Element = nextElement(&wpt2Iter);

        while (wpt2Element != NULL){
            Waypoint* waypoint2 = (Waypoint*) wpt2Element;
            if(waypoint2->name != NULL && strlen(waypoint2->name) != 0){
                numData++;
            }
            if(waypoint2->otherData != NULL){
                numData+=(waypoint2->otherData->length);
            }
            wpt2Element = nextElement(&wpt2Iter);
        }

        routeElement = nextElement(&routeIter);
    }

    while (trackElement != NULL){

        Track* track = (Track*) trackElement;

        if (track->name != NULL && strlen(track->name) != 0){
            numData++;
        }

        if (track->otherData != NULL) {
            numData+=(track->otherData->length);
        }

        ListIterator segIter = createIterator(track->segments);
        void* segElement = nextElement(&segIter);

        while (segElement != NULL){

            TrackSegment* segment = (TrackSegment*) segElement;
            
            ListIterator wpt3Iter = createIterator(segment->waypoints);
            void* wpt3Element = nextElement(&wpt3Iter);

            while(wpt3Element != NULL){

                Waypoint* waypoint3 = (Waypoint*) wpt3Element;
                if (waypoint3->name != NULL && strlen(waypoint3->name) != 0){
                    numData++;
                }
                if(waypoint3->otherData != NULL){
                    numData+=(waypoint3->otherData->length);
                }

                wpt3Element = nextElement(&wpt3Iter);
            }       
            segElement = nextElement(&segIter);

        }
        trackElement = nextElement(&trackIter);
    }
    
    return numData;
}

// Function that returns a waypoint with the given name.  If more than one exists, return the first one.  
// Return NULL if the waypoint does not exist
Waypoint* getWaypoint(const GPXdoc* doc, char* name){

    if (doc == NULL || name == NULL){
        return NULL;
    }

    ListIterator listIter = createIterator(doc->waypoints);
    void* listElement = nextElement(&listIter);

    while (listElement != NULL){
        Waypoint* waypoint = (Waypoint*) listElement;

        if(strcmp(waypoint->name, name) == 0){
            return waypoint;
        }

        listElement = nextElement(&listIter);
    }

    return NULL;

}
// Function that returns a track with the given name.  If more than one exists, return the first one. 
// Return NULL if the track does not exist 
Track* getTrack(const GPXdoc* doc, char* name){

    if (doc == NULL || name == NULL){
        return NULL;
    }

    ListIterator listIter = createIterator(doc->tracks);
    void* listElement = nextElement(&listIter);

    while (listElement != NULL){
        Track* track = (Track*) listElement;

        if(strcmp(track->name, name) == 0){
            return track;
        }
        listElement = nextElement(&listIter);
    }

    return NULL;

}
// Function that returns a route with the given name.  If more than one exists, return the first one.  
// Return NULL if the route does not exist
Route* getRoute(const GPXdoc* doc, char* name){

    if (doc == NULL || name == NULL){
        return NULL;
    }

    ListIterator listIter = createIterator(doc->routes);
    void* listElement = nextElement(&listIter);

    while (listElement != NULL){
        Route* route = (Route*) listElement;

        if(strcmp(route->name, name) == 0){
            return route;
        }
        listElement = nextElement(&listIter);
    }

    return NULL;
}

// A2 FUNCTIONS MODULE 1
/** Function to create an GPX object based on the contents of an GPX file.
 * This function must validate the XML tree generated by libxml against a GPX schema file
 * before attempting to traverse the tree and create an GPXdoc struct
 *@pre File name cannot be an empty string or NULL.
       File represented by this name must exist and must be readable.
 *@post Either:
        A valid GPXdoc has been created and its address was returned
		or 
		An error occurred, and NULL was returned
 *@return the pinter to the new struct or NULL
 *@param gpxSchemaFile - the name of a schema file
 *@param fileName - a string containing the name of the GPX file
**/
GPXdoc* createValidGPXdoc(char* fileName, char* gpxSchemaFile){
    if (fileName == NULL || gpxSchemaFile == NULL){
        return NULL;
    }

    // if (strcmp(strrchr(fileName, '.'), ".gpx") != 0){
    //     return false;
    // }
    LIBXML_TEST_VERSION
    
    xmlDocPtr doc;
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;
    char *XMLFileName = fileName;
    char *XSDFileName = gpxSchemaFile;

    xmlLineNumbersDefault(1);

    ctxt = xmlSchemaNewParserCtxt(XSDFileName);

    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);
    //xmlSchemaDump(stdout, schema); //To print schema dump

    doc = xmlReadFile(XMLFileName, NULL, 0);

    if (doc == NULL)
    {
        fprintf(stderr, "Could not parse %s\n", XMLFileName);
    }
    else
    {
        xmlSchemaValidCtxtPtr ctxt;
        int ret;

        ctxt = xmlSchemaNewValidCtxt(schema);
        xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
        ret = xmlSchemaValidateDoc(ctxt, doc);
        if (ret == 0)
        {
            // printf("%s validates\n", XMLFileName);
        }
        else if (ret > 0)
        {
            fprintf(stderr, "%s fails to validate\n", XMLFileName);
        }
        else
        {
            fprintf(stderr, "%s validation generated an internal error\n", XMLFileName);
        }
        xmlSchemaFreeValidCtxt(ctxt);
        xmlFreeDoc(doc);
    }

    // free the resource
    if(schema != NULL) {
        xmlSchemaFree(schema);

        xmlSchemaCleanupTypes();
        xmlCleanupParser();
        xmlMemoryDump();

    }
    //I malloc within the function
    GPXdoc *newGPXdoc = createGPXdoc(fileName);

    // xmlFreeDoc(doc);
    // xmlCleanupParser();

    return newGPXdoc;
}

//M1 HELPER FUNCTIONS
// filename for output file?
xmlDoc* convertGPXtoXML(GPXdoc *gpxDoc){

    xmlDocPtr doc = NULL;
    xmlNodePtr root_node = NULL;

    xmlNodePtr node = NULL;

    // xmlNodePtr child_node = NULL;

    // xmlNodePtr data_node = NULL;

    xmlNodePtr seg_node = NULL;

    // xmlNewNs namespace = NULL;

    LIBXML_TEST_VERSION;

    // create new document and node which is set to root node

    char version[20];// = ftoa(gpxDoc->version);

    sprintf(version, "%.1f", gpxDoc->version);

    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "gpx");
    xmlDocSetRootElement(doc, root_node);

    

    xmlNewProp(root_node, BAD_CAST "version", BAD_CAST version);
    xmlNewProp(root_node, BAD_CAST "creator", BAD_CAST gpxDoc->creator);

    //namespace NOT SURE (always on top)
    xmlNsPtr ns_ptr = xmlNewNs(root_node, BAD_CAST gpxDoc->namespace, NULL);
    // xmlNsPtr ns1_ptr = xmlNewNs(root_node, BAD_CAST "http://www.w3.org/2001/XMLSchema-instance", BAD_CAST "xsi");
    // xmlNsPtr ns2_ptr = xmlNewNs(root_node, BAD_CAST gpxDoc->namespace, BAD_CAST "");

    xmlSetNs(root_node, ns_ptr);
    // xmlSetNs(root_node, ns1_ptr);

    // xmlNewProp(root_node, BAD_CAST "xmlns:xsi", BAD_CAST "http://www.w3.org/2001/XMLSchema-instance");
    // xmlNewProp(root_node, BAD_CAST "xmlns", BAD_CAST gpxDoc->namespace);
    // xmlNewProp(root_node, BAD_CAST "xsi:schemaLocation", BAD_CAST "http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd");
    

    ListIterator listIter = createIterator(gpxDoc->waypoints);
    void* listElement = nextElement(&listIter);

    while (listElement != NULL){

        Waypoint* waypoint = (Waypoint*) listElement;

        char lon[20];
        char lat[20];

        sprintf(lat, "%f", waypoint->latitude);
        sprintf(lon, "%f", waypoint->longitude);

        node = xmlNewChild(root_node, NULL, BAD_CAST "wpt", NULL);
        xmlNewProp(node, BAD_CAST "lat", BAD_CAST lat);
        xmlNewProp(node, BAD_CAST "lon", BAD_CAST lon);

        if (waypoint->name[0] != '\0'){
            xmlNewChild(node,NULL, BAD_CAST "name", BAD_CAST waypoint->name);
        }
        

        //go through other data

        ListIterator dataIter = createIterator(waypoint->otherData);
            
        void* dataElement = nextElement(&dataIter);

        while (dataElement != NULL){
            GPXData* data = (GPXData*) dataElement;
            xmlNewChild(node, NULL, BAD_CAST data->name, BAD_CAST data->value);
            dataElement = nextElement(&dataIter);
        }

        listElement = nextElement(&listIter);
    }

    listIter = createIterator(gpxDoc->routes);
    listElement = nextElement(&listIter);

    xmlNodePtr wpt_node = NULL;
    // xmlNodePtr wpt_child_node = NULL;

    while (listElement != NULL){

        Route* route = (Route*) listElement;

        node = xmlNewChild(root_node, NULL, BAD_CAST "rte", NULL);

        // // if ()
        if (route->name[0] != '\0'){
            xmlNewChild(node,NULL, BAD_CAST "name", BAD_CAST route->name);
        }

        // other data
        // child_node = xmlNewChild(node,NULL, BAD_CAST "desc", BAD_CAST route->desc);

        ListIterator dataIter = createIterator(route->otherData);
            
        void* dataElement = nextElement(&dataIter);

        while (dataElement != NULL){
            GPXData* data = (GPXData*) dataElement;
            xmlNewChild(node, NULL, BAD_CAST data->name, BAD_CAST data->value);
            dataElement = nextElement(&dataIter);
        }

        //CREATE ANOTHER INTERATOR AND GO THROUGH WAYPOINTS (rtept)

        ListIterator wayIter = createIterator(route->waypoints);
        void* wayElement = nextElement(&wayIter);

        while (wayElement != NULL){
            Waypoint* waypoint = (Waypoint*) wayElement;

            char lon[20];
            char lat[20];   

            sprintf(lat, "%f", waypoint->latitude);
            sprintf(lon, "%f", waypoint->longitude);

            wpt_node = xmlNewChild(node, NULL, BAD_CAST "rtept", NULL);
            xmlNewProp(wpt_node, BAD_CAST "lat", BAD_CAST lat);
            xmlNewProp(wpt_node, BAD_CAST "lon", BAD_CAST lon);

            //check if name is there

            if (waypoint->name[0] != '\0'){
                xmlNewChild(wpt_node,NULL, BAD_CAST "name", BAD_CAST waypoint->name);
            }

            //go through other data

            dataIter = createIterator(waypoint->otherData);
            
            dataElement = nextElement(&dataIter);

            while (dataElement != NULL){
                GPXData* data = (GPXData*) dataElement;
                xmlNewChild(wpt_node, NULL, BAD_CAST data->name, BAD_CAST data->value);
                dataElement = nextElement(&dataIter);
            }
            // 

            wayElement = nextElement(&wayIter);

        }      

        listElement = nextElement(&listIter);
    }

    //TRACK SEGMENTS

    listIter = createIterator(gpxDoc->tracks);
    listElement = nextElement(&listIter);

    seg_node = NULL;
    // xmlNodePtr seg_child_node = NULL;

    while (listElement != NULL){

        Track* track = (Track*) listElement;

        node = xmlNewChild(root_node, NULL, BAD_CAST "trk", NULL);

        // // if ()
        if (track->name[0] != '\0'){
            xmlNewChild(node,NULL, BAD_CAST "name", BAD_CAST track->name);
        }

        //other data
        // child_node = xmlNewChild(node,NULL, BAD_CAST "desc", BAD_CAST route->desc);

        ListIterator dataIter = createIterator(track->otherData);
            
        void* dataElement = nextElement(&dataIter);

        while (dataElement != NULL){
            GPXData* data = (GPXData*) dataElement;
            xmlNewChild(node, NULL, BAD_CAST data->name, BAD_CAST data->value);
            dataElement = nextElement(&dataIter);
        }


        //CREATE ANOTHER INTERATOR AND GO THROUGH sgements (rtept)

        ListIterator segIter = createIterator(track->segments);
        void* segElement = nextElement(&segIter);

        while (segElement != NULL){
            TrackSegment* segment = (TrackSegment*) segElement;

            // char lon[20];
            // char lat[20];   

            // sprintf(lat, "%.5f", waypoint->latitude);
            // sprintf(lon, "%.5f", waypoint->longitude);

            seg_node = xmlNewChild(node, NULL, BAD_CAST "trkseg", NULL);
            // xmlNewProp(seg_node, BAD_CAST "lat", BAD_CAST lat);
            // xmlNewProp(seg_node, BAD_CAST "lon", BAD_CAST lon);

            //check if name is there

            // if (strcmp(waypoint->name, "") == 0){
            // seg_child_node = xmlNewChild(seg_node, NULL, BAD_CAST "name", segment->name);
            // }

            //go through other data

            ListIterator wayIter = createIterator(segment->waypoints);
            void* wayElement = nextElement(&wayIter);

            while (wayElement != NULL){
                Waypoint* waypoint = (Waypoint*) wayElement;

                char lon[20];
                char lat[20];   

                sprintf(lat, "%f", waypoint->latitude);
                sprintf(lon, "%f", waypoint->longitude);

                wpt_node = xmlNewChild(seg_node, NULL, BAD_CAST "trkpt", NULL);
                xmlNewProp(wpt_node, BAD_CAST "lat", BAD_CAST lat);
                xmlNewProp(wpt_node, BAD_CAST "lon", BAD_CAST lon);

                //check if name is there

                if (waypoint->name[0] != '\0'){
                    xmlNewChild(wpt_node,NULL, BAD_CAST "name", BAD_CAST waypoint->name);
                }

                //go through other data

                dataIter = createIterator(waypoint->otherData);
                
                dataElement = nextElement(&dataIter);

                while (dataElement != NULL){
                    GPXData* data = (GPXData*) dataElement;
                    xmlNewChild(wpt_node, NULL, BAD_CAST data->name, BAD_CAST data->value);
                    dataElement = nextElement(&dataIter);
                }
                // 

                wayElement = nextElement(&wayIter);

            }
            
            // 

            segElement = nextElement(&segIter);

        }

        listElement = nextElement(&listIter);
    }


    
    // xmlNewChild(root_node, NULL, BAD_CAST "gpx",BAD_CAST "content of node 1");

    // xmlSaveFormatFileEnc("output.gpx", doc, "UTF-8", 1);

    // xmlFreeDoc(doc);
    xmlCleanupParser();

    return doc;

}

//not complete MEMORY LEAK
bool validateXMLTree(xmlDoc* doc, char* gpxSchemaFile){

    if (doc == NULL || gpxSchemaFile == NULL){
        return false;
    }

    // xmlDoc* xDoc = NULL;

    char *XSDFileName = gpxSchemaFile;

    LIBXML_TEST_VERSION

    xmlSchemaParserCtxtPtr ctxt;    
    xmlSchemaPtr schema = NULL;

    xmlLineNumbersDefault(1);

    ctxt = xmlSchemaNewParserCtxt(XSDFileName);

    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schema = xmlSchemaParse(ctxt); //MEM LEAK
    xmlSchemaFreeParserCtxt(ctxt);

    int ret;

    if (doc == NULL){
		fprintf(stderr, "Could not parse \n");
	}
	else{
		xmlSchemaValidCtxtPtr ctxt;
 
		ctxt = xmlSchemaNewValidCtxt(schema);
		xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
		ret = xmlSchemaValidateDoc(ctxt, doc);
 
		if (ret == 0){
			// printf(" validates\n");
		}
		else if (ret > 0){
			printf(" fails to validate\n");
            return false;
		}
		else{
			printf(" validation generated an internal error\n");
            return false;
		}
		xmlSchemaFreeValidCtxt(ctxt);
		// xmlFreeDoc(doc);
	}

    if(schema != NULL){
		xmlSchemaFree(schema);
    }   

    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    xmlMemoryDump();

    return true;

}

//validate helper functions
bool validateWaypoints(List *waypoints){
    if (waypoints == NULL){
        return false;
    }

    ListIterator wptIter = createIterator(waypoints);
    void* wptElem = nextElement(&wptIter);

    Waypoint *waypoint;// = (Waypoint*)wptElem;

    while (wptElem != NULL){
        waypoint = (Waypoint*)wptElem;

        if(waypoint->name == NULL){
            return false;
        }
        if (waypoint->otherData == NULL){
            return false;
        }

        wptElem = nextElement(&wptIter);
    }
    return true;
}

bool validateRoutes(List *routes){
    if (routes == NULL){
        return false;
    }

    ListIterator routeIter = createIterator(routes);
    void* routeElem = nextElement(&routeIter);
    Route *route;

    while (routeElem != NULL){
        route = (Route*)routeElem;

        if(route->name == NULL){
            return false;
        }

        if (route->otherData == NULL){
            return false;
        }

        if (validateWaypoints(route->waypoints) == false){
            return false;
        }

        routeElem = nextElement(&routeIter);
    }

    return true;

}

bool validateTracks(List *tracks){
    if (tracks == NULL){
        return false;
    }

    ListIterator trackIter = createIterator(tracks);
    void* trackElem = nextElement(&trackIter);
    Track *track;

    while (trackElem != NULL){
        track = (Track*)trackElem;

        if (track->name == NULL){
            return false;
        }
        if (track->otherData == NULL){
            return false;
        }

        if (track->segments == NULL){
            return false;
        } else {
            ListIterator segIter = createIterator(track->segments);
            void* segElem = nextElement(&segIter);
            TrackSegment *seg;

            while (segElem != NULL){

                seg = (TrackSegment*)segElem;

                if (validateWaypoints(seg->waypoints) == false){
                    return false;
                }

                segElem = nextElement(&segIter);                
            }
        }

        trackElem = nextElement(&trackIter);
    }

    return true;
}

/** Function to validating an existing a GPXobject object against a GPX schema file
 *@pre 
    GPXdoc object exists and is not NULL
    schema file name is not NULL/empty, and represents a valid schema file
 *@post GPXdoc has not been modified in any way
 *@return the boolean aud indicating whether the GPXdoc is valid
 *@param doc - a pointer to a GPXdoc struct
 *@param gpxSchemaFile - the name of a schema file
 **/
bool validateGPXDoc(GPXdoc* doc, char* gpxSchemaFile){

    if (doc == NULL || gpxSchemaFile == NULL){
        return false;
    }

    xmlDoc* xDoc = NULL;

    xDoc = convertGPXtoXML(doc);
    if (xDoc == NULL){
        return NULL;
    }

    //MEM LEAK
    if (validateXMLTree(xDoc, gpxSchemaFile) == false){
        return false;
    }
    
    xmlFreeDoc(xDoc);

    //go through all content within the gpxdoc and see if it is valid (initiated)

    //check the namespace and creator
    if (doc->namespace == NULL || doc->creator == NULL){
        return false;
    }

    //go through waypoints
    if (validateWaypoints(doc->waypoints) == false){
        return false;
    }

    if (validateRoutes(doc->routes) == false){
        return false;
    }

    if (validateTracks(doc->tracks) == false){
        return false;
    }
   

    return true;

}

int validateGPXJSON(char *fileName, char* XSDFileName){
    if (fileName == NULL || XSDFileName == NULL){
        return 0;
    }

    GPXdoc *newGPXdoc = createGPXdoc(fileName);
    if (validateGPXDoc(newGPXdoc,XSDFileName) == true){
        return 1;
    }

    return 0;
}

bool writeGPXdoc(GPXdoc* doc, char* fileName){
    if (doc == NULL || fileName == NULL){
        return false;
    }

    if (strcmp(strrchr(fileName, '.'), ".gpx") != 0){
        return false;
    }
    xmlDocPtr xDoc = convertGPXtoXML(doc);

    xmlSaveFormatFileEnc(fileName, xDoc, "UTF-8", 1);

    xmlFreeDoc(xDoc);
    xmlCleanupParser();

    return true;
}

//Module 2

//HAVERSINE FORMULA

float haversineCalc(Waypoint *w1, Waypoint *w2){

    //earth radius in metres
    float earthRadius = 6371000;

    //lon and lat of waypoints in radians
    float lat1 = w1->latitude * M_PI/180;
    float lat2 = w2->latitude * M_PI/180;

    float lon1 = w1->longitude * M_PI/180;
    float lon2 = w2->longitude * M_PI/180;

    float dLat = (lat2 - lat1);
    float dLon = (lon2 - lon1);

    float a = sin(dLat/2) * sin(dLat/2) + cos(lat1) * cos(lat2) * sin(dLon/2) * sin(dLon/2);

    float c = 2 * atan2(sqrt(a), sqrt(1-a));
    
    float d = earthRadius * c;

    return d;    

}

//lat 1 lon 1 then lat 2 lon 2
float haversineDistCalc(float lati1, float long1, float lati2, float long2){

    //earth radius in metres
    float earthRadius = 6371000;

    //lon and lat of waypoints in radians
    float lat1 = lati1 * M_PI/180;
    float lat2 = lati2 * M_PI/180;

    float lon1 = long1 * M_PI/180;
    float lon2 = long2 * M_PI/180;

    float dLat = (lat2 - lat1);
    float dLon = (lon2 - lon1);

    float a = sin(dLat/2) * sin(dLat/2) + cos(lat1) * cos(lat2) * sin(dLon/2) * sin(dLon/2);

    float c = 2 * atan2(sqrt(a), sqrt(1-a));
    
    float d = earthRadius * c;

    return d;

}

/** Function that returns the length of a Route
**/
float getRouteLen(const Route *rt){
    if (rt == NULL){
        return 0;
    }
    float len = 0;
    
    ListIterator listIter = createIterator(rt->waypoints);
    void* listElem = nextElement(&listIter);    

    Waypoint *w1 = (Waypoint*) listElem;
    

    while (listElem != NULL){
        // printf("%f\n",len);
        Waypoint *temp = w1;
        
        listElem = nextElement(&listIter);
        if (listElem != NULL){
            w1 = (Waypoint*) listElem;
          
            len += haversineCalc(w1, temp);
        }
    }

    return len;
}

/** Function that returns the length of a Track
**/
float getTrackLen(const Track *tr){
    if (tr == NULL){
        return 0;
    }

    float len = 0;

    //go through each segment within the track
    ListIterator segIter = createIterator(tr->segments);
    void* segElem = nextElement(&segIter);
    

    while (segElem != NULL){
        TrackSegment *seg = (TrackSegment*) segElem;

        //go through each waypoint and add results

        ListIterator listIter = createIterator(seg->waypoints);
        void* listElem = nextElement(&listIter);    

        Waypoint *w1 = (Waypoint*) listElem;
        

        while (listElem != NULL){
            // printf("%f\n",len);
            Waypoint *temp = w1;
            
            listElem = nextElement(&listIter);
            if (listElem != NULL){
                w1 = (Waypoint*) listElem;
            
                len += haversineCalc(w1, temp);
            }
        }


        segElem = nextElement(&segIter);

    }

    return len;

}

/** Function that rounds the length of a track or a route to the nearest 10m
**/
float round10(float len){
    int roundNum = (int) len;
    int roundTen = roundNum % 10;
    return (roundTen < 5 ? roundNum - roundTen : roundNum + 10 - roundTen);
}

/** Function that returns the number routes with the specified length, using the provided tolerance 
 * to compare route lengths
**/
int numRoutesWithLength(const GPXdoc* doc, float len, float delta){
    if (doc == NULL || len < 0 || delta < 0){
        return 0;
    }

    float routeLen = 0;
    int count = 0;

    ListIterator listIter = createIterator(doc->routes);
    void* listElem = nextElement(&listIter);    

    // Route *route = (Route*) listElem;
    

    while (listElem != NULL){
        // printf("%f\n",len);
        Route *route = (Route*) listElem;
        routeLen = getRouteLen(route);

        //check this 
        if ( (routeLen >= len - delta) && (routeLen <= len + delta) ){
            count++;
        }
        listElem = nextElement(&listIter);
    } 

    return count;

}


/** Function that returns the number tracks with the specified length, using the provided tolerance 
 * to compare track lengths
**/
int numTracksWithLength(const GPXdoc* doc, float len, float delta){
    if (doc == NULL || len < 0 || delta < 0){
        return 0;
    }

    float trackLen = 0;
    int count = 0;

    ListIterator listIter = createIterator(doc->tracks);
    void* listElem = nextElement(&listIter);    

    // Track *track = (Track*) listElem;
    

    while (listElem != NULL){
        // printf("%f\n",len);
        Track *track = (Track*) listElem;
        trackLen = getTrackLen(track);

        float difference = abs(trackLen - len);

        //check this 
        if ( difference <= delta ){
            count++;
        }
        listElem = nextElement(&listIter);
    } 

    return count;
}

/** Function that checks if the current route is a loop
**/
bool isLoopRoute(const Route* route, float delta){

    if (route == NULL || delta < 0){
        return false;
    }

    if (route->waypoints->length < 4){
        return false;
    }

    Waypoint *firstPoint = (Waypoint*)getFromFront(route->waypoints);
    Waypoint *lastPoint = (Waypoint*)getFromBack(route->waypoints);

    //check if distance bewteen first and last points are less then delta
    // ListIterator listIter = createIterator(route->waypoints);
    // void* listElem = nextElement(&listIter);    

    // Waypoint *firstPoint = (Waypoint*) listElem;    
    // Waypoint *lastPoint;

    // while (listElem != NULL){
        
    //     if (listElem != NULL){
    //         lastPoint = (Waypoint*) listElem;
    //     }        
        
    //     listElem = nextElement(&listIter);
        
    // }

    float distance = haversineCalc(firstPoint, lastPoint);

    if (distance < delta){
        return true;
    }

    return false;

}


/** Function that checks if the current track is a loop
**/
bool isLoopTrack(const Track *tr, float delta){
    if (tr == NULL || delta < 0){
        return false;
    }
    // if (route->waypoints->length < 4){
    //     return false;
    // }

    int wypNum = 0;

    //check if distance bewteen first and last points are less then delta

    ListIterator segIter = createIterator(tr->segments);
    void* segElem = nextElement(&segIter);        

    while (segElem != NULL){
        TrackSegment *segment = (TrackSegment*) segElem;
        wypNum += segment->waypoints->length;
        segElem = nextElement(&segIter);
    }    

    if (wypNum < 4){
        return false;
    }

    TrackSegment *firstSeg = (TrackSegment*)getFromFront(tr->segments);
    TrackSegment *lastSeg = (TrackSegment*)getFromBack(tr->segments);

    Waypoint *firstPoint = (Waypoint*)getFromFront(firstSeg->waypoints);
    Waypoint *lastPoint = (Waypoint*)getFromBack(lastSeg->waypoints);

    float distance = haversineCalc(firstPoint, lastPoint);

    if (distance < delta){
        return true;
    }

    return false;
}

void dummyDelete(void* data){
}

/** Function that returns all routes between the specified start and end locations
 *@pre GPXdoc object exists, is not null
 *@post GPXdoc object exists, is not null, has not been modified
 *@return a list of Route structs that connect the given sets of coordinates
 *@param doc - a pointer to a GPXdoc struct
 *@param sourceLat - latitude of the start location
 *@param sourceLong - longitude of the start location
 *@param destLat - latitude of the destination location
 *@param destLong - longitude of the destination location
 *@param delta - the tolerance used for comparing distances between waypoints 
*/
List* getRoutesBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta){

    if (doc == NULL){
        return NULL;
    }

    //doc->routes;
    List* routesBetween = initializeList(&routeToString,&dummyDelete,&compareRoutes);

    //check beginning and ending waypoints of routes and check if they are close to source and dest lat and lon
    
    //go through each route

    ListIterator listIter = createIterator(doc->routes);
    void* listElem = nextElement(&listIter);    
    // Route *route = (Route*) listElem;
    Route *route;

    float sourceDistance = 0;
    float destDistance = 0;

    int addedAlready = 0;

    while (listElem != NULL){
        addedAlready = 0;
        // printf("%f\n",len);
        route = (Route*) listElem;

        Waypoint* firstPoint = (Waypoint*)getFromFront(route->waypoints);
        Waypoint* lastPoint = (Waypoint*)getFromBack(route->waypoints);

        //gotta check both cases

        sourceDistance = haversineDistCalc(sourceLat, sourceLong, firstPoint->latitude, firstPoint->longitude);
        destDistance = haversineDistCalc(destLat, destLong, lastPoint->latitude, lastPoint->longitude);

        if (sourceDistance <= delta && destDistance <= delta){
            insertBack(routesBetween, route);
            addedAlready = 1;
        }

        sourceDistance = haversineDistCalc(sourceLat, sourceLong, lastPoint->latitude, lastPoint->longitude);
        destDistance = haversineDistCalc(destLat, destLong, firstPoint->latitude, firstPoint->longitude);

        if (sourceDistance <= delta && destDistance <= delta && addedAlready != 1){
            insertBack(routesBetween, route);
            // addedAlready = 1;
        }
        
        listElem = nextElement(&listIter);
    } 

    if (routesBetween->length == 0){
        return NULL;
    }

    return routesBetween;

}

/** Function that returns all Tracks between the specified start and end locations
 *@pre GPXdoc object exists, is not null
 *@post GPXdoc object exists, is not null, has not been modified
 *@return a list of Track structs that connect the given sets of coordinates
 *@param doc - a pointer to a GPXdoc struct
 *@param sourceLat - latitude of the start location
 *@param sourceLong - longitude of the start location
 *@param destLat - latitude of the destination location
 *@param destLong - longitude of the destination location
 *@param delta - the tolerance used for comparing distances between waypoints 
*/
List* getTracksBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta){

    if (doc == NULL){
        return NULL;
    }

    //doc->routes;
    List* tracksBetween = initializeList(&trackToString,&dummyDelete,&compareTracks);

    ListIterator listIter = createIterator(doc->tracks);
    void* listElem = nextElement(&listIter);    
    // Route *route = (Route*) listElem;
    Track *track;

    float sourceDistance = 0;
    float destDistance = 0;

    int addedAlready = 0;

    //iterate through tracks
    while (listElem != NULL){
        addedAlready = 0;
        track = (Track*) listElem;

        
        //check first waypoint of the first segment and check last waypoint of the last segment
        TrackSegment *firstSeg = (TrackSegment*)getFromFront(track->segments);
        TrackSegment *lastSeg = (TrackSegment*)getFromBack(track->segments);

        Waypoint *firstPoint = (Waypoint*)getFromFront(firstSeg->waypoints);
        Waypoint *lastPoint = (Waypoint*)getFromBack(lastSeg->waypoints);

        sourceDistance = haversineDistCalc(sourceLat, sourceLong, firstPoint->latitude, firstPoint->longitude);
        destDistance = haversineDistCalc(destLat, destLong, lastPoint->latitude, lastPoint->longitude);

        if (sourceDistance <= delta && destDistance <= delta){
            insertBack(tracksBetween, track);
            addedAlready = 1;
        }

        sourceDistance = haversineDistCalc(sourceLat, sourceLong, lastPoint->latitude, lastPoint->longitude);
        destDistance = haversineDistCalc(destLat, destLong, firstPoint->latitude, firstPoint->longitude);

        if (sourceDistance <= delta && destDistance <= delta && addedAlready != 1){
            insertBack(tracksBetween, track);
            // addedAlready = 1;
        }
        
        listElem = nextElement(&listIter);
    } 

    if (tracksBetween->length == 0){
        return NULL;
    }

    return tracksBetween;

}


//A2 Module 3

char* gpxDataToJSON(const GPXData *data){

    char* dataStr;
    if (data == NULL){
        //memory allocate
        dataStr = (char*)malloc(sizeof(char)*3);
        strcpy(dataStr,"{}");
        return dataStr;
    }

    char* name;
    char* value;

    if (strcmp(data->name, "") != 0){
        name = (char*)malloc(strlen(data->name));
        strcpy(name,data->name);
    } else {
        name = (char*)malloc(5);
        strcpy(name,"None");
    }

    if (strcmp(data->value, "") != 0){
        value = (char*)malloc(strlen(data->value));
        strtok(value, "\n");
        strcpy(value,data->value);
    } else {
        value = (char*)malloc(5);
        strcpy(value,"None");
    }

    dataStr = (char *)malloc(sizeof(char) * (strlen(value) + strlen(data->name) + 57));

    sprintf(dataStr,"{\"name\":\"%s\",\"value\":\"%s\"}", name, value);

    free(name);
    free(value);

    return dataStr;
}

char* gpxDataListToJSON(const List *list){
    char* jsonStr;

    if (list == NULL || list->length == 0){
        jsonStr = (char*)malloc(sizeof(char)*3);
        strcpy(jsonStr,"[]");
        return jsonStr;
    }

    int mallocSize = 0;
    int commaCount = 0;

    ListIterator listIter = createIterator((List*)list);
    void *listElem = nextElement(&listIter);
    GPXData *data;// = (Route*)routeElem;

    char *temp;

    while (listElem != NULL){
        data = (GPXData*)listElem;

        temp = gpxDataToJSON(data);
        
        mallocSize += strlen(temp);
        listElem = nextElement(&listIter);
        commaCount++;

        free(temp);
    }
    //add [] and end + commas
    jsonStr = (char *)malloc(mallocSize + 3 + commaCount);

    strcpy(jsonStr,"[");

    ListIterator listIter2 = createIterator((List *)list);
    listElem = nextElement(&listIter2);

    while (listElem != NULL){
        data = (GPXData*)listElem;

        temp = gpxDataToJSON(data);

        strcat(jsonStr, temp);

        listElem = nextElement(&listIter2);

        if (listElem != NULL){
            strcat(jsonStr, ",");
        }
        free(temp);
    }

    strcat(jsonStr, "]");

    // free(temp);

    // printf("%s\n", jsonStr);

    return jsonStr;
}

/** Function to converting a Track into a JSON string
 **/
char* trackToJSON(const Track *tr){

    char* trackStr;
    if (tr == NULL){
        //memory allocate
        trackStr = (char*)malloc(sizeof(char)*3);
        strcpy(trackStr,"{}");
        return trackStr;
    }

    char* loopBool;
    char* name;

    if (isLoopTrack(tr,10) == true){
        loopBool = (char *)malloc(5);
        strcpy(loopBool, "true");
    } else{
        loopBool = (char *)malloc(6);
        strcpy(loopBool,"false");
    }
    if (strcmp(tr->name, "") != 0){
        name = (char*)malloc(strlen(tr->name));
        strcpy(name,tr->name);
    } else {
        name = (char*)malloc(5);
        strcpy(name,"None");
    }

    

    //go through each segment and add length of waypoints of each segment
    ListIterator segIter = createIterator(tr->segments);
    void* segElem = nextElement(&segIter);
    int numPoints = 0;

    while (segElem != NULL){ 
        TrackSegment *seg = (TrackSegment*) segElem;

        numPoints+=seg->waypoints->length;

        segElem = nextElement(&segIter);
    }

    char* otherDataStr = gpxDataListToJSON(tr->otherData);
    // printf("%s\n", otherDataStr);

    trackStr = (char *)malloc(sizeof(char) * (strlen(loopBool) + strlen(tr->name) + strlen(otherDataStr) + 60));

    sprintf(trackStr,"{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1f,\"loop\":%s,\"otherData\":%s}", name, numPoints, round10(getTrackLen(tr)), loopBool, otherDataStr);

    free(loopBool);
    free(name);
    free(otherDataStr);

    return trackStr;
}

/** Function to converting a Route into a JSON string
 **/
char* routeToJSON(const Route *rt){

    char* routeStr;

    if (rt == NULL){
        //memory allocate
        routeStr = (char*)malloc(sizeof(char)*3);
        strcpy(routeStr,"{}");
        return routeStr;
    }

    // char* routeStr;

    char* loopBool;
    char* name;

    if (isLoopRoute(rt,10) == true){
        loopBool = (char *)malloc(5);
        strcpy(loopBool, "true");
    } else{
        loopBool = (char *)malloc(6);
        strcpy(loopBool,"false");
    }

    if (strcmp(rt->name, "") != 0){
        name = (char*)malloc(strlen(rt->name));
        strcpy(name, rt->name);
    } else {
        name = (char*)malloc(5);
        strcpy(name,"None");
    }

    char* otherDataStr = gpxDataListToJSON(rt->otherData);

    routeStr = (char *)malloc(sizeof(char) * (strlen(loopBool) + strlen(name) + strlen(otherDataStr) + 60));

    

    sprintf(routeStr,"{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1f,\"loop\":%s,\"otherData\":%s}", name, rt->waypoints->length, round10(getRouteLen(rt)), loopBool, otherDataStr);

    free(loopBool);
    free(name);
    free(otherDataStr);

    // printf("%s\n", routeStr );

    return routeStr;
}



// CONST LIST CREATES WARNINGS WITH ITERATOR / FIX THIS

/** Function to converting a list of Route structs into a JSON string
 **/
char* routeListToJSON(const List *list){
    char* routeStr;

    if (list == NULL || list->length == 0){
        //memory allocate
        routeStr = (char*)malloc(sizeof(char)*3);
        strcpy(routeStr,"[]");
        return routeStr;
    }

    int mallocSize = 0;
    int commaCount = 0;

    ListIterator routeIter = createIterator((List *)list);
    void *routeElem = nextElement(&routeIter);
    Route *route;// = (Route*)routeElem;

    char *temp;

    while (routeElem != NULL){
        route = (Route*)routeElem;

        temp = routeToJSON(route);

        mallocSize += strlen(temp);
        routeElem = nextElement(&routeIter);
        commaCount++;

        free(temp);
    }
    //add [] and end
    routeStr = (char *)malloc(mallocSize + 3 + commaCount);

    strcpy(routeStr,"[");

    ListIterator route2Iter = createIterator((List *)list);
    routeElem = nextElement(&route2Iter);

    while (routeElem != NULL){
        route = (Route*)routeElem;

        temp = routeToJSON(route);

        strcat(routeStr, temp);

        routeElem = nextElement(&route2Iter);

        if (routeElem != NULL){
            strcat(routeStr, ",");
        }

        free(temp);
    }

    strcat(routeStr, "]");
    // free(temp);

    return routeStr;
}

/** Function to converting a list of Track structs into a JSON string
 **/
char* trackListToJSON(const List *list){
    char* trackStr;
    if (list == NULL || list->length == 0){
        //memory allocate
        trackStr = (char*)malloc(sizeof(char)*3);
        strcpy(trackStr,"[]");
        return trackStr;
    }

    int mallocSize = 0;
    int commaCount = 0;

    ListIterator trackIter = createIterator((List*)list);
    void *trackElem = nextElement(&trackIter);
    Track *track;// = (Route*)routeElem;

    char *temp;

    while (trackElem != NULL){
        track = (Track*)trackElem;

        temp = trackToJSON(track);
        
        mallocSize += strlen(temp);
        trackElem = nextElement(&trackIter);
        commaCount++;

        free(temp);
    }
    //add [] and end + commas
    trackStr = (char *)malloc(mallocSize + 3 + commaCount);

    strcpy(trackStr,"[");

    ListIterator track2Iter = createIterator((List *)list);
    trackElem = nextElement(&track2Iter);

    while (trackElem != NULL){
        track = (Track*)trackElem;

        temp = trackToJSON(track);

        strcat(trackStr, temp);

        trackElem = nextElement(&track2Iter);

        if (trackElem != NULL){
            strcat(trackStr, ",");
        }
        free(temp);
    }

    strcat(trackStr, "]");

    // free(temp);

    return trackStr;
}

char* waypointToJSON(const Waypoint *wpt){
    char* wptStr;

    if (wpt == NULL){
        //memory allocate
        wptStr = (char*)malloc(sizeof(char)*3);
        strcpy(wptStr,"{}");
        return wptStr;
    }

    char* name;

    if (strcmp(wpt->name, "") != 0){
        name = (char*)malloc(strlen(wpt->name));
        strcpy(name, wpt->name);
    } else {
        name = (char*)malloc(5);
        strcpy(name,"None");
    }

    // char* otherDataStr = gpxDataListToJSON(wpt->otherData);

    wptStr = (char *)malloc(sizeof(char) * (strlen(name) + 60));

    

    sprintf(wptStr,"{\"name\":\"%s\",\"lat\":%f,\"lon\":%f}", name, wpt->latitude, wpt->longitude);

    free(name);

    // printf("%s\n", routeStr );

    return wptStr;
}

char* waypointListToJSON(List *list){
    char* wptStr;

    if (list == NULL || list->length == 0){
        //memory allocate
        wptStr = (char*)malloc(sizeof(char)*3);
        strcpy(wptStr,"[]");
        return wptStr;
    }

    int mallocSize = 0;
    int commaCount = 0;

    ListIterator wptIter = createIterator((List *)list);
    void *wptElem = nextElement(&wptIter);
    Waypoint *wpt;// = (Route*)routeElem;

    char *temp;

    while (wptElem != NULL){
        wpt = (Waypoint*)wptElem;

        temp = waypointToJSON(wpt);

        mallocSize += strlen(temp);
        wptElem = nextElement(&wptIter);
        commaCount++;

        free(temp);
    }
    //add [] and end
    wptStr = (char *)malloc(mallocSize + 3 + commaCount);

    strcpy(wptStr,"[");

    ListIterator wpt2Iter = createIterator((List *)list);
    wptElem = nextElement(&wpt2Iter);

    while (wptElem != NULL){
        wpt = (Waypoint*)wptElem;

        temp = waypointToJSON(wpt);

        strcat(wptStr, temp);

        wptElem = nextElement(&wpt2Iter);

        if (wptElem != NULL){
            strcat(wptStr, ",");
        }

        free(temp);
    }

    strcat(wptStr, "]");
    // free(temp);

    return wptStr;
}

char* waypointListJSON (char* fileName, char* routeName) {

    char* jsonStr;

    GPXdoc *doc = createGPXdoc(fileName);

    int found = 0;

    if (validateGPXDoc(doc, "parser/src/gpx.xsd") == true){

        //iterate through routes until found matching routename and get waypoint data

        ListIterator routeIter = createIterator(doc->routes);
        void *routeElem = nextElement(&routeIter);
        Route *route;// = (Route*)routeElem;

        while (routeElem != NULL){
            route = (Route*)routeElem;

            if(strcmp(route->name, routeName) == 0){
                jsonStr = waypointListToJSON(route->waypoints);
                found = 1;
            }

            routeElem = nextElement(&routeIter);
        }

        

    }
    if(found == 0){
        jsonStr = malloc(3);
        strcpy(jsonStr, "[]");
    }

    deleteGPXdoc(doc);

    return jsonStr;

}

char* routeListJSON(char* fileName){

    char* compStr;

    // printf("check 1\n");
    GPXdoc *doc = createGPXdoc(fileName);
    // printf("check 2\n");

    if (validateGPXDoc(doc, "parser/src/gpx.xsd") == true){
        // printf("check 3\n");
        compStr = routeListToJSON(doc->routes);
        // printf("check 4\n");
    } else{
        // printf("check 5\n");
        compStr = malloc(3);
        strcpy(compStr, "[]");
    }

    // printf("check 6\n");

    deleteGPXdoc(doc);

    // printf("check 7\n");

    return compStr;
}

char* trackListJSON(char* fileName){

    char* compStr;

    GPXdoc *doc = createGPXdoc(fileName);

    if (validateGPXDoc(doc, "parser/src/gpx.xsd") == true){
        compStr = trackListToJSON(doc->tracks);
    } else {
        compStr = malloc(3);
        strcpy(compStr, "[]");
    }

    deleteGPXdoc(doc);

    return compStr;
}

// char* allRouteOtherDataJSON(char* fileName){
//     char* compStr;

//     // GPXdoc *doc = createGPXdoc(fileName);

    

    

//     return compStr;
// }


char* otherDataListJSON(char* fileName){
    char* compStr;

    GPXdoc *doc = createGPXdoc(fileName);

    if (validateGPXDoc(doc, "parser/src/gpx.xsd") == true){

        compStr = gpxDataListToJSON(doc->tracks);
    } else {
        compStr = malloc(3);
        strcpy(compStr, "[]");
    }
    deleteGPXdoc(doc);
    return compStr;
}
/** Function to converting a GPXdoc into a JSON string
 **/
char* GPXtoJSON(const GPXdoc* gpx){
    char *gpxStr;

    if (gpx == NULL){
        //memory allocate
        gpxStr = (char*)malloc(sizeof(char) *3);
        strcpy(gpxStr,"{}");
        return gpxStr;
    }

    
    gpxStr = (char *)malloc(sizeof(char) * strlen(gpx->creator) + 92);

    sprintf(gpxStr,"{\"version\":%.1f,\"creator\":\"%s\",\"numWaypoints\":%d,\"numRoutes\":%d,\"numTracks\":%d}", gpx->version, gpx->creator, gpx->waypoints->length, gpx->routes->length, gpx->tracks->length);

    return gpxStr;
}

char* gpxJSON(char* fileName) {

    char *gpxStr;

    if (fileName == NULL){
        gpxStr = (char*)malloc(sizeof(char) *3);
        strcpy(gpxStr,"{}");
        return gpxStr;
    }

    GPXdoc *doc = createGPXdoc(fileName);

    gpxStr = GPXtoJSON(doc);

    deleteGPXdoc(doc);

    return gpxStr;
}

// send in filepath as filename
int renameComponent(char* fileName, char* newName, char* oldName){

    if (fileName == NULL || newName == NULL || oldName == NULL){
        return 0;
    }

    GPXdoc *doc = createGPXdoc(fileName);

    if (validateGPXDoc(doc, "parser/src/gpx.xsd") == false){
        deleteGPXdoc(doc);
        return 0;
    }

    // char* filePath = (char*)malloc(sizeof(char*) * (strlen("uploads/") + strlen(fileName)));

    // strcpy(filePath, "uploads/");
    // strcat(filePath, fileName);

    // go through the doc and iterate through routes and tracks to find oldName
    // then replace it and write a newgpxdoc

    ListIterator routeIter = createIterator(doc->routes);
    void* routeElem = nextElement(&routeIter);    

    // Route *route = (Route*) listElem;
    

    while (routeElem != NULL){
        // printf("%f\n",len);
        Route *route = (Route*) routeElem;

        if(strcmp(route->name, oldName) == 0){
            route->name = (char *)malloc(strlen(newName));
            strcpy(route->name,newName);

            if(writeGPXdoc(doc, fileName) == false){
                deleteGPXdoc(doc);
                return 0;
            }
            deleteGPXdoc(doc);
            return 1;
        }
        
        routeElem = nextElement(&routeIter);
    } 

    ListIterator trackIter = createIterator(doc->tracks);
    void* trackElem = nextElement(&trackIter);    

    // Route *route = (Route*) listElem;
    

    while (trackElem != NULL){
        // printf("%f\n",len);
        Track *track = (Track*) trackElem;

        if(strcmp(track->name, oldName) == 0){
            track->name = (char *)malloc(strlen(newName));
            strcpy(track->name,newName);

            if(writeGPXdoc(doc, fileName) == false){
                deleteGPXdoc(doc);
                return 0;
            }
            deleteGPXdoc(doc);
            return 1;
        }
        
        trackElem = nextElement(&trackIter);
    } 

    deleteGPXdoc(doc);
    

    return 0;

}

int renameProper(char* fileName, char* newName, char* compType, int compNum){

    if (fileName == NULL || newName == NULL || compType == NULL){
        return 0;
    }

    GPXdoc *doc = createGPXdoc(fileName);

    if (validateGPXDoc(doc, "parser/src/gpx.xsd") == false){
        deleteGPXdoc(doc);
        return 0;
    }

    // char* filePath = (char*)malloc(sizeof(char*) * (strlen("uploads/") + strlen(fileName)));

    // strcpy(filePath, "uploads/");
    // strcat(filePath, fileName);

    // go through the doc and iterate through routes and tracks to find oldName
    // then replace it and write a newgpxdoc

    if (strcmp(compType, "Route") == 0){

        ListIterator routeIter = createIterator(doc->routes);
        void* routeElem = nextElement(&routeIter);

        for (int i = 0; i < compNum; i++){
            Route *route = (Route*) routeElem;
            if (i == compNum-1){
                route->name = (char *)malloc(strlen(newName));
                strcpy(route->name,newName);

                if(writeGPXdoc(doc, fileName) == false){
                    deleteGPXdoc(doc);
                    return 0;
                }
                deleteGPXdoc(doc);
                return 1;
            }
            routeElem = nextElement(&routeIter);
        }
        
    } 
    else if(strcmp(compType, "Track") == 0){
        ListIterator trackIter = createIterator(doc->tracks);
        void* trackElem = nextElement(&trackIter);

        for (int i = 0; i < compNum; i++){
            Track *track = (Track*) trackElem;
            if (i == compNum-1){
                track->name = (char *)malloc(strlen(newName));
                strcpy(track->name,newName);

                if(writeGPXdoc(doc, fileName) == false){
                    deleteGPXdoc(doc);
                    return 0;
                }
                deleteGPXdoc(doc);
                return 1;
            }
            trackElem = nextElement(&trackIter);
        }
    }


    deleteGPXdoc(doc);
    

    return 0;

}



// char* gpxJSON(char* fileName) {

//     char *gpxStr;

//     if (fileName == NULL){
//         gpxStr = (char*)malloc(sizeof(char) *3);
//         strcpy(gpxStr,"{}");
//         return gpxStr;
//     }

//     GPXdoc *doc = createGPXdoc(fileName);

//     gpxStr = GPXtoJSON(doc);

//     return gpxStr;
// }



// ***************************** Bonus A2 functions ********************************

/** Function to adding an Waypont struct to an existing Route struct
 **/
void addWaypoint(Route *rt, Waypoint *pt){
    if (rt == NULL || pt == NULL){
        return;
    }

    insertBack(rt->waypoints, pt);

}

/** Function to adding an Route struct to an existing GPXdoc struct
 **/
void addRoute(GPXdoc* doc, Route* rt){
    if (doc == NULL || rt == NULL){
        return;
    }

    insertBack(doc->routes, rt);
}

/** Function to converting a JSON string into an GPXdoc struct
 *@pre JSON string is not NULL
 *@post String has not been modified in any way
 *@return A newly allocated and initialized GPXdoc struct
 *@param str - a pointer to a string
 **/
GPXdoc* JSONtoGPX(const char* gpxString){

    if (gpxString == NULL){
        return NULL;
    }
    int i;
    // int size = 256;

    char versionStr[256] = "";
    char creator[256] = "";

    //get string upto first colon
    char *splitString = strchr(gpxString, ':');
    //remove colon
    splitString++;

    //find comma to see where it ends
    int endIndex = 0;
    for (i=0; i< strlen(splitString); i++){
        if (splitString[i] == ','){
            endIndex = i;
        }
    }

    //this copies correct area into versionStr with the ending index
    strncpy(versionStr, splitString, endIndex);

    splitString = strchr(splitString, ':');
    //remove colon and quote
    splitString+=2;

    strncpy(creator, splitString, strlen(splitString) - 3);


    double version = atof(versionStr);

    // printf("version: %.1f\n", version);
    // printf("creator: %s\n", creator);

    GPXdoc *doc = (GPXdoc*)malloc(sizeof(GPXdoc));

    doc->version = version;

    doc->creator = (char *)malloc(strlen(creator) + 1);
    strcpy(doc->creator, creator);

    strcpy(doc->namespace,"http://www.topografix.com/GPX/1/1");

    //initialize lists
    doc->waypoints = initializeList(&waypointToString,&deleteWaypoint,&compareWaypoints);
    doc->routes = initializeList(&routeToString,&deleteRoute,&compareRoutes);
    doc->tracks = initializeList(&trackToString,&deleteTrack,&compareTracks);

    // free(create);

    // free(splitString);
    // free(verStr);
    // free(creator);

    return doc;
}

//create gpx file
int jsonGPX(char* gpxString, char* fileName){
    if (gpxString == NULL || fileName == NULL){
        return 0;
    }

    GPXdoc *doc = JSONtoGPX(gpxString);

    if (writeGPXdoc(doc, fileName) == true){
        return 1;
    }

    return 0;
}


/** Function to converting a JSON string into an Waypoint struct
 *@pre JSON string is not NULL
 *@post String has not been modified in any way
 *@return A newly allocated and initialized Waypoint struct
 *@param str - a pointer to a string
 **/
Waypoint* JSONtoWaypoint(const char* gpxString){
    if (gpxString == NULL){
        return NULL;
    }

    int i;

    char latVal[256] = "";
    char lonVal[256] = "";

    // take information from JSON string here
    // {"lat":latVal,"lon":lonVal}

    //get lat first
    char *splitString = strchr(gpxString, ':');
    splitString++;

    int endIndex = 0;

    for (i=0; i< strlen(splitString); i++){
        if (splitString[i] == ','){
            endIndex = i;
        }
    }

    strncpy(latVal, splitString, endIndex);

    // printf("%s\n", latVal);

    splitString = strchr(splitString, ':');
    splitString++;

    strncpy(lonVal, splitString, strlen(splitString) - 1);

    // printf("%s\n", lonVal);
    double lat = atof(latVal);
    double lon = atof(lonVal);

    Waypoint *wpt = (Waypoint*)malloc(sizeof(Waypoint));

    wpt->latitude = lat;
    wpt->longitude = lon;

    wpt->otherData = initializeList(&gpxDataToString,&deleteGpxData,&compareGpxData);

    wpt->name = (char *)malloc(1);
    strcpy(wpt->name,"\0");

    // printf("%s\n",waypointToString(wpt));

    return wpt;
}

/** Function to converting a JSON string into an Route struct
 *@pre JSON string is not NULL
 *@post String has not been modified in any way
 *@return A newly allocated and initialized Route struct
 *@param str - a pointer to a string
 **/
Route* JSONtoRoute(const char* gpxString){
    if (gpxString == NULL){
        return NULL;
    }

    Route *rt = (Route*)malloc(sizeof(Route));

    char name[256] = "";

    //take information from JSON string
    char *splitString = strchr(gpxString,':');
    splitString+=2;

    strncpy(name, splitString, strlen(splitString) - 2);

    // printf("%s\n", name);

    rt->name = (char *)malloc(strlen(name) + 1);

    strcpy(rt->name, name);
    rt->waypoints = initializeList(&waypointToString,&deleteWaypoint,&compareWaypoints);
    rt->otherData = initializeList(&gpxDataToString,&deleteGpxData,&compareGpxData);

    // printf("%s\n", routeToString(rt));

    return rt;

}

int addJSONwaypoint(char* fileName, char* waypointJSON){

    GPXdoc *doc = createGPXdoc(fileName);

    if (validateGPXDoc(doc, "parser/src/gpx.xsd") == false){
        deleteGPXdoc(doc);
        return 0;
    }

    Waypoint *wpt = JSONtoWaypoint(waypointJSON);

    Route *rt = (Route*)getFromBack(doc->routes);
    addWaypoint(rt, wpt);

    

    writeGPXdoc(doc, fileName);

    deleteGPXdoc(doc);

    return 1;


}

//first add route, then add waypoint
int addJSONroute(char* fileName, char* routeJSON){

    GPXdoc *doc = createGPXdoc(fileName);

    if (validateGPXDoc(doc, "parser/src/gpx.xsd") == false){
        deleteGPXdoc(doc);
        return 0;
    }

    Route *rt = JSONtoRoute(routeJSON);
    if (rt == NULL){
        return 0;
    }

    addRoute(doc, rt);

    

    writeGPXdoc(doc, fileName);
    deleteGPXdoc(doc);

    return 1;


}



/**
 * Simple example to parse a file called "file.xml", 
 * walk down the DOM, and print the name of the 
 * xml elements nodes.
 */
// int
// main(int argc, char **argv)
// {
//     // xmlDoc *doc = NULL;
//     // xmlNode *root_element = NULL;

//     GPXdoc *newGPXdoc;//= (GPXdoc*)malloc(sizeof(GPXdoc));

//     if (argc != 3){
//         printf("usage: xmlExample2 <someXMLfile> <xsd file>\n");

//         return(1);
//     }

//     //GPXdoc newGPXdoc = malloc(sizeof(GPXdoc));
//     // newGPXdoc = createGPXdoc(argv[1]);

//     newGPXdoc = createValidGPXdoc(argv[1], argv[2]);

//     // xmlDoc* xDoc = convertGPXtoXML(newGPXdoc);

//     //check validation of xmlTree //doesnt work

//     // if (validateXMLTree(xDoc, argv[2])){
//     //     printf("true\n");
//     // } else {
//     //     printf("false\n");
//     // }

//     if (writeGPXdoc(newGPXdoc, "testOutput.gpx") == false){
//         printf("write error\n");
//     }

//     // ListIterator routeIter = createIterator(newGPXdoc -> routes);
   
//     // void *routeElem = nextElement(&routeIter);
     
//     // Route *route = (Route*)routeElem;

//     // char *routeawd = routeToJSON(route);
//     // printf("%s\n", routeawd);
//     // free(routeawd);
    
//     // printf("%f\n", getRouteLen(route));

//     // ListIterator trackIter = createIterator(newGPXdoc -> tracks);

//     // void *trackElement = nextElement(&trackIter);
     
//     // Track *track = (Track*)trackElement;

//     // char *trackawd = trackToJSON(track);
//     // printf("%s\n", trackawd);
//     // free(trackawd);

//     // char* routeListJSON = routeListToJSON(newGPXdoc->routes);
//     // printf("%s\n", routeListJSON);
//     // free(routeListJSON);

//     // char* trackListJSON = trackListToJSON(newGPXdoc->tracks);
//     // printf("%s\n", trackListJSON);
//     // free(trackListJSON);

//     // if (validateGPXDoc(newGPXdoc, argv[2])){
//     //     printf("true\n");
//     // } else {
//     //     printf("false\n");
//     // }

//     // List *routes = getRoutesBetween(newGPXdoc, 44.595850, -123.620950, 44.597150, -123.620220, 0.0);

//     // if (routes != NULL){

//     //     char *rList = routeListToJSON(routes);

//     //     printf("%s\n", rList);
//     //     free(rList);

//     //     char *routeStrings = toString(routes);

//     //     printf("%s\n", routeStrings);

//     //     freeList(routes);
//     //     free(routeStrings);
//     // }

//     // List *tracks = getTracksBetween(newGPXdoc, 49.595850, -123.620950, 49.598350, -123.619520, 0.0);
    
//     // if (tracks != NULL){

//     //     // char *tlist = trackListToJSON(tracks);

//     //     // printf("%s\n", tlist);
//     //     // free(tlist);

//     //     char *trackString = toString(tracks);

//     //     printf("%s\n", trackString);

//     //     freeList(tracks);
//     //     free(trackString);
        
//     // }

    

//     // ListIterator routeIter = createIterator(newGPXdoc -> routes);
   
//     // void *routeElem = nextElement(&routeIter);
     
//     // Route *route = (Route*)routeElem;

//     // char *routeawd = routeToJSON(route);
//     // printf("%s\n", routeawd);
//     // free(routeawd);
    
//     // printf("%f\n", getRouteLen(route));

//     // ListIterator trackIter = createIterator(newGPXdoc -> tracks);

//     // void *trackElement = nextElement(&trackIter);
     
//     // Track *track = (Track*)trackElement;

//     // char *trackawd = trackToJSON(track);
//     // printf("%s\n", trackawd);
//     // free(trackawd);

//     // printf("%f\n", getTrackLen(track));

    

//     // void *waypointElement1;
//     // void *waypointElement2;
//     // ListIterator waypointIterator = createIterator(newGPXdoc -> waypoints);
//     // waypointElement1 = nextElement(&waypointIterator);
//     // waypointElement2 = nextElement(&waypointIterator);

//     // Waypoint *waypoint1 = (Waypoint*)waypointElement1;
//     // Waypoint *waypoint2 = (Waypoint*)waypointElement2;

//     // printf("lat: %f lon: %f\nlat: %f lon: %f\n",waypoint1->latitude, waypoint1->longitude, waypoint2->latitude, waypoint2->longitude);
//     // printf("%f\n", haversineCalc(waypoint1, waypoint2));

//     // printWaypoints(newGPXdoc);

//     // printf("num waypoints: \t%d\n", getNumWaypoints(newGPXdoc));
//     // printf("num routes: \t%d\n", getNumRoutes(newGPXdoc));
//     // printf("num tracks: \t%d\n", getNumTracks(newGPXdoc));
//     // printf("num segments: \t%d\n", getNumSegments(newGPXdoc));
//     // printf("num data: \t%d\n", getNumGPXData(newGPXdoc));

//     // GPXdoc *testDoc = JSONtoGPX("{\"version\":1.1,\"creator\":\"creatorValue\")}");

//     // Waypoint *testWpt = JSONtoWaypoint("{\"lat\":43.537299,\"lon\":-80.218267}");

//     // Route *testRt = JSONtoRoute("{\"name\":\"Reynolds walk\"}");

//     // addWaypoint(testRt, testWpt);
//     // addRoute(testDoc, testRt);

//     // printf("%s\n", GPXdocToString(testDoc));

//     // deleteWaypoint(testWpt);
//     // deleteRoute(testRt);
//     // deleteGPXdoc(testDoc);

//     // printf(newGPXdoc->namespace);
//     // xmlFreeDoc(doc);
//     // xmlCleanupParser();

//     // printf(GPXdocToString(newGPXdoc));

//     deleteGPXdoc(newGPXdoc);
    
//     // printf("final doc check\n");

//     return 0;
// }