//
// Created by vityha on 19.04.22.
//

mergeDicts(){
    // get dicts
    dict1 = getDict();
    if dict1 == "" return;
    dict2 = getDict();
    if dict2 == "" return;

    // merge dicts

    // enque result
    dictsQueue.enque(resultDict);
}

getDict(){
    dict = dictsQueue.deque();
    if dict == ""
    {
        if numOfIndexers == 0
        {
            dictsQueue.enque("");
            return dict;
        }
        // insert poisson pill -> numOfIndexers--
        else{
            dict = dictsQueue.deque();
        }
    }
    return dict
}

---------------------------------------------

// read file and enque
readFiles(){
    //read file in struct
    //enque
}

1. create struct
2. function