#pragma once
template <typename T>
class List {
public:

    struct Elements {
        Elements(T value1) {
            value = value1;
            next = NULL;
        }

        T value;
        Elements *next;
    };

    List() {
        end = NULL;
        begin = NULL;
    }
    ~List() {
        while(begin) {
            Elements *slot = begin;
            begin   = begin->next;
            delete slot;
        }
    }
    Elements *find(T value) {
        Elements *slot = begin;
        while (slot) {
            if (slot->value == value)
                return slot;
            slot = slot->next;
        }
        return NULL;
    }
    void push(T value) {
        if (end == NULL) {
            end = new Elements(value);
            begin = end;
        }
        else
        {
            end->next = new Elements(value);
            end = end->next;
        }
    }
    void remove(T key) {
       Elements *slot = find(key);
       if (!slot)
           return;
       Elements *previous = begin;
       if (end == begin) {
           delete end;
           end = NULL;
           begin = NULL;
           return;
       }
       while (previous) {
           if (previous->next == slot)
               break;
           previous = previous->next;
       }
       // если предыдущего нет, значит это первый
       if (!previous) {
           Elements *memory = begin;
           begin = begin->next;
           delete memory;
       }
       else {
            if (slot == end) {
                end = previous;
                end->next = NULL;
                delete slot;
            }
            else {
                previous->next = slot ->next;
                delete slot;
            }
       }


    }

    Elements *begin;
    Elements *end;
};
