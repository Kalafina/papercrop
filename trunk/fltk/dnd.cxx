// A trival example illustration how to load and send and image
// using Drag and Drop events.
//
// Step 1. Compile using: flt-config --use-images --compile dndtest1.cxx
//
// Step 2. Run: dndtest1
//
// Step 3. Drag an image from your favourite file browser onto the
//         marked box.
//
// Step 4. Drag the image from the engraved area onto your favourite file
//         browser.

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Shared_Image.H> // Image I/O
#include <FL/fl_ask.H>          // fl_alert, fl_choice
#include <FL/filename.H>        // fl_filename_name
#include <string.h>             // strncmp, strlen, etc.
#include <stdio.h>              // snprintf


class Fl_DND_Box : public Fl_Box
{
    public:
        static void callback_deferred(void *v)
        {
            Fl_DND_Box *w = (Fl_DND_Box*)v;

            w->do_callback();
        }

        Fl_DND_Box(int X, int Y, int W, int H, const char *L = 0)
                : Fl_Box(X,Y,W,H,L), evt(FL_NO_EVENT), evt_txt(0), evt_len(0)
        {
            labeltype(FL_NO_LABEL);
            box(FL_NO_BOX);
            clear_visible_focus();
        }

        virtual ~Fl_DND_Box()
        {
            delete [] evt_txt;
        }

        int event()
        {
            return evt;
        }

        const char* event_text()
        {
            return evt_txt;
        }

        int event_length()
        {
            return evt_len;
        }


        int handle(int e)
        {
            switch(e)
            {
                /* Initiating Drag and Drop */
                case FL_PUSH:
                    evt = e;

                    // Indicate that this widget wants FL_DRAG
                    if(callback() && (when() & FL_WHEN_CHANGED))
                        do_callback();
                    return 1;

                case FL_DRAG:
                    evt = e;

                    // Indicate the the LMB is pressed and mouse is in motion.
                    // User typically initiates the DND at this point
                    if(callback() && (when() & FL_WHEN_CHANGED))
                        do_callback();
                    return 1;

                case FL_RELEASE:
                    evt = e;

                    // Indicate the the LMB is not pressed. Dragging is done
                    if(callback() && (when() & FL_WHEN_CHANGED))
                        do_callback();
                    return 1;


                /* Receiving Drag and Drop */
                case FL_DND_ENTER:
                case FL_DND_RELEASE:
                case FL_DND_LEAVE:
                case FL_DND_DRAG:
                    evt = e;
                    return 1;

                case FL_PASTE:
                    evt = e;

                    // make a copy of the DND payload
                    evt_len = Fl::event_length();

                    delete [] evt_txt;

                    evt_txt = new char[evt_len];
                    strcpy(evt_txt, Fl::event_text());

                    // If there is a callback registered, call it.
                    // The callback must access Fl::event_text() to
                    // get the string or file path(s) that was dropped.
                    // Note that do_callback() is not called directly.
                    // Instead it will be executed by the FLTK main-loop
                    // once we have finished handling the DND event.
                    // This allows caller to popup a window or change widget focus.
                    if(callback() && ((when() & FL_WHEN_RELEASE) || (when() & FL_WHEN_CHANGED)))
                        Fl::add_timeout(0.0, Fl_DND_Box::callback_deferred, (void*)this);
                    return 1;
            }

            return Fl_Box::handle(e);
        }

    protected:
        // The event which caused Fl_DND_Box to execute its callback
        int evt;

        char *evt_txt;
        int evt_len;
};



// Widget group that displays the image. Fl_Group holds
// the Fl_Box and the Fl_DND_Box. This way Fl_DND_Box
// will resize according to Fl_Box
Fl_Group *box_group = (Fl_Group*)0;
Fl_Box *box = (Fl_Box*)0;

void file_close()
{
    Fl_Shared_Image *img = (Fl_Shared_Image*)box->image();

    if(!img)
        return; // no image displayed

    box->image(0);
    
    // The image is shared, release until no more references
    while(img->refcount())
        img->release();
}

void file_open(const char *fpath)
{
    file_close();

    Fl_Shared_Image *img = Fl_Shared_Image::get(fpath);
    
    if(!img)
    {
        fl_alert("Failed to load image: %s", fpath);
        return;
    }

    // Resizing box_group will resize the Fl_Box and Fl_DND_Box widgets
    box_group->size(img->w(), img->h());
    box->image(img);

    Fl::redraw();
}

void dnd_receive(const char *urls)
{
    // count number of files dropped
    int cnt = 0;

    for(const char *c = urls; *c != '\0'; ++c)
    {
        if(*c == '\n')
            ++cnt;
    }

    if(cnt != 1)
    {
        fl_alert("Invalid number of files being dropped: %d.\nDrop 1 file only.", cnt);
        return;
    }
    
    if(strncmp(urls, "file://", 7) != 0)
    {
        fl_alert("Unsupported URL: %s\nOnly local files are supported in this demo.", urls);
        return;
    }

    // Fl::event_text() gives URLs in the format: file:///path/to/image\r\n
    // which is not supported by Fl_Shared_Image. Therefore, we extract
    // the /path/to/image

    // -7 == file://  -2 == \r\n
    int fsz = strlen(urls) - 7 - 2;
    char *fpath = new char[fsz+1];

    // copy only what we need
    strncpy(fpath, urls+7, fsz);
    fpath[fsz] = '\0';

    // NOTE: fpath may contain URL escape codes which we really should decode.
    //       For this example, they are ignored.
    if(fl_choice("Do you really want to open image: %s?", "&No", "&Yes", 0, fl_filename_name(fpath)) == 1)
        file_open(fpath);

    delete [] fpath;
}

void dnd_initiate()
{
    // box contains a shared image so we put the path to that image into
    // the selection buffer
    Fl_Shared_Image *img = (Fl_Shared_Image*)box->image();

    // Construct a URL. Though not necessay for some apps, others
    // may be expecting the file path to be formated as a URL.
    // In either case, URL + \r\n is used for receiving DND so
    // we will just be consistent with that.
    // +7 == "file://" +2 == "\r\n" +1 == '\0'
    int sz    = strlen(img->name()) + 7 + 2 + 1;
    char *url = new char[sz];

    snprintf(url, sz, "file://%s\r\n", img->name());

    // Copy the image's file path to the selection buffer
    Fl::copy(url, strlen(url)+1, 0);

    // initiate the DnD magic.
    // At this point FLTK's system wide DnD kicks in and we lose all control
    Fl::dnd();

    delete [] url;
}

void dnd_cb(Fl_Widget *o, void *v)
{
    Fl_DND_Box *dnd = (Fl_DND_Box*)o;

    switch(dnd->event())
    {
        case FL_DRAG:           // User is dragging widget
            if(box->image() && Fl::event_button1()) // Only do something if box has an image and LMB is pressed
                dnd_initiate();
            break;

        case FL_PASTE:          // DND Drop
            dnd_receive(dnd->event_text());
            break;
    }
}

int main(int argc, char *argv[])
{
    Fl::visual(FL_DOUBLE | FL_INDEX);
    Fl::get_system_colors();
    fl_register_images();
    Fl::scheme("gtk+");

    Fl_Double_Window *wnd = new Fl_Double_Window(10, 10, 500, 445, "Receiving and Initiating DND Example");
    {
        {
            Fl_Box *o = new Fl_Box(15, 15, 470, 90,
            "1. Drag an image from your favourite file browser onto the area below.\n"
            "2.Drag the image off of the demo and into your favourite file browser.");
            o->box(FL_ROUNDED_BOX);
            o->align(FL_ALIGN_INSIDE | FL_ALIGN_WRAP| FL_ALIGN_LEFT);
            o->color((Fl_Color)215);
            o->labelfont(FL_HELVETICA_BOLD);
        }

        {
            Fl_Scroll *o = new Fl_Scroll(15, 115, 470, 320, 0);
            o->box(FL_ENGRAVED_BOX);
            {
                {
                    box_group = new Fl_Group(17, 117, 466, 316, 0);
                    box_group->box(FL_NO_BOX);

                        box = new Fl_Box(17, 117, 466, 316, 0);
                        box->box(FL_FLAT_BOX);
                        box->align(FL_ALIGN_INSIDE | FL_ALIGN_CENTER);

                    { // Fl_DND_Box is constructed with the same dimensions and at the same position as Fl_Box
                        Fl_DND_Box *o = new Fl_DND_Box(17, 117, 466, 316, 0);
                        o->when(FL_WHEN_CHANGED | FL_WHEN_RELEASE);
                        o->callback(dnd_cb);
                    }

                    box_group->end();
                }
            }
            o->end();
            Fl_Group::current()->resizable(o);
        }
    }
    wnd->end();
    wnd->show(argc, argv);

    return Fl::run();
}
