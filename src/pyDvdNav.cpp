#define PY_SSIZE_T_CLEAN  /* Make "s#" use Py_ssize_t rather than int. */

#include <Python.h>

#include "dvdnav/dvdnav.h"
#include "DvdNav.h"

#include <tuple>

#define DVD_READ_CACHE 1
#define DVD_LANGUAGE "en"

extern "C" {
    typedef struct {
        PyObject_HEAD
        DvdNav dvdnav;
        uint8_t mem[DVD_VIDEO_LB_LEN];
        int finished;
        int output_fd;
        int dump;
        int tt_dump;
    } dvdNav;

    static int DvdNav_init(dvdNav *self, PyObject *args, PyObject *kwds) {
        static char *kwlist[] = {"isoName", NULL};
        Py_buffer *isoName = NULL;

        if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &isoName))
            return -1;
        printf("Opening iso %s...\n", (char *) isoName);

        /* open dvdnav handle */
        try {
            self->dvdnav = DvdNav((char *) isoName);
        } catch (std::exception& e) {
            printf("shit\n");
            printf("%s\n", e.what());
            return -1;
        }

        return 0;
    }

    static PyObject *DvdNav_next_block(dvdNav *self, PyObject *Py_UNUSED(ignored)) {


        blockTuple result;

        /* the main reading function */
        try {
            result = self->dvdnav.getNextBlock();
        } catch (const DvdNavException& e) {
            PyErr_SetString(PyExc_Exception, e.getMessage().c_str());
            return NULL;
        } catch (std::exception &e) {
            PyErr_SetString(PyExc_Exception, "Unknown Exception");
            return NULL;
        }

        return PyTuple_Pack(2, PyLong_FromLongLong(std::get<1>(result)), PyLong_FromLongLong(std::get<2>(result)));
    }

    /** DvdNav_still_skip
     *
     *
     * @param self
     * @param args
     * @return the length of the still event
     */
    static PyObject *DvdNav_still_skip(dvdNav *self, PyObject *args) {

        auto *event = reinterpret_cast<dvdnav_still_event_t *>(self->dvdnav.getMem());

        PyObject *length = PyLong_FromLongLong(event->length);

        try {
            self->dvdnav.stillSkip();
        } catch (const DvdNavException& e) {
            PyErr_SetString(PyExc_Exception, e.getMessage().c_str());
            return NULL;
        } catch (std::exception &e) {
            PyErr_SetString(PyExc_Exception, "Unknown Exception");
            return NULL;
        }

        return length;
    }

    /**
     * If we are currently in WAIT state, that is: the application is required to wait for its fifos to become empty,
     * calling this signals libdvdnav that this is achieved and that it can continue.
     *
     * @param self
     * @param args
     * @return None
     */
    static PyObject *DvdNav_wait_skip(dvdNav *self, PyObject *args) {
        try {
            self->dvdnav.waitSkip();
        } catch (const DvdNavException& e) {
            PyErr_SetString(PyExc_Exception, e.getMessage().c_str());
            return NULL;
        } catch (std::exception &e) {
            PyErr_SetString(PyExc_Exception, "Unknown Exception");
            return NULL;
        }

        Py_RETURN_NONE;
    }

    /** DvdNav_get_highlight
     *
     * @param self
     * @param args
     * @return
     */
    static PyObject *DvdNav_get_highlight(dvdNav *self, PyObject *args) {
        auto *highlight_event = reinterpret_cast<dvdnav_highlight_event_t *>(self->dvdnav.getMem());

        return PyTuple_Pack(
                8,
                PyLong_FromLongLong(highlight_event->buttonN),
                PyLong_FromLongLong(highlight_event->display),
                PyLong_FromLongLong(highlight_event->ex),
                PyLong_FromLongLong(highlight_event->ey),
                PyLong_FromLongLong(highlight_event->pts),
                PyLong_FromLongLong(highlight_event->palette),
                PyLong_FromLongLong(highlight_event->sx),
                PyLong_FromLongLong(highlight_event->sy)
        );

    }

    /** DvdNav_current_title_info
     *
     * @param self
     * @param args
     * @return
     */
    static PyObject *DvdNav_current_title_info(dvdNav *self, PyObject *args) {

        try {
            currentTitleInfoTuple currentTitleInfo = self->dvdnav.getCurrentTitleInfo();
            return PyTuple_Pack(2, PyLong_FromLongLong(std::get<0>(currentTitleInfo)),
                                PyLong_FromLongLong(std::get<1>(currentTitleInfo)));
        } catch (const DvdNavException& e) {
            PyErr_SetString(PyExc_Exception, e.getMessage().c_str());
            return NULL;
        } catch (std::exception &e) {
            PyErr_SetString(PyExc_Exception, "Unknown Exception");
            return NULL;
        }
    }


    /** dvdnav_current_title_program
     *
     * @param self
     * @param args
     * @return
     */
    static PyObject *DvdNav_dvdnav_current_title_program(dvdNav *self, PyObject *args) {

        try {
            currentTitleProgramTuple currentTitleProgram = self->dvdnav.getCurrentTitleProgram();
            return PyTuple_Pack(
                    3,
                    PyLong_FromLongLong(std::get<0>(currentTitleProgram)),
                    PyLong_FromLongLong(std::get<1>(currentTitleProgram)),
                    PyLong_FromLongLong(std::get<2>(currentTitleProgram))
                    );
        } catch (const DvdNavException& e) {
            PyErr_SetString(PyExc_Exception, e.getMessage().c_str());
            return NULL;
        } catch (std::exception &e) {
            PyErr_SetString(PyExc_Exception, "Unknown Exception");
            return NULL;
        }
    }

    /** DvdNav_get_position
     *
     * @param self
     * @param args
     * @return
     */
    static PyObject *DvdNav_get_position(dvdNav *self, PyObject *args) {
        try {
            positionTuple position = self->dvdnav.getPosition();
            return PyTuple_Pack(23,
                    PyLong_FromLongLong(std::get<0>(position)),
                    PyLong_FromLongLong(std::get<1>(position))
            );
        } catch (const DvdNavException& e) {
            PyErr_SetString(PyExc_Exception, e.getMessage().c_str());
            return NULL;
        } catch (std::exception &e) {
            PyErr_SetString(PyExc_Exception, "Unknown Exception");
            return NULL;
        }
    }

    /** DvdNav_get_current_nav_buttons
     *
     * @param self
     * @param args
     * @return
     */
    static PyObject *DvdNav_get_current_nav_buttons(dvdNav *self, PyObject *args) {
        pci_t pci = self->dvdnav.getCurrentNavPCI();
        dsi_t dsi = self->dvdnav.getCurrentNavDSI();

        if (pci.hli.hl_gi.btn_ns > 0) {
            int button;

            PyObject *buttons = PyTuple_New(pci.hli.hl_gi.btn_ns);

            printf("Found %i DVD menu buttons...\n", pci.hli.hl_gi.btn_ns);

            for (button = 0; button < pci.hli.hl_gi.btn_ns; button++) {
                const btni_t *btni = &(pci.hli.btnit[button]);

                PyObject *buttonPts = PyDict_New();
                PyDict_SetItem(buttonPts, PyUnicode_FromString("x_start"), PyLong_FromLongLong(btni->x_start));
                PyDict_SetItem(buttonPts, PyUnicode_FromString("x_end"), PyLong_FromLongLong(btni->x_end));
                PyDict_SetItem(buttonPts, PyUnicode_FromString("y_start"), PyLong_FromLongLong(btni->y_start));
                PyDict_SetItem(buttonPts, PyUnicode_FromString("y_end"), PyLong_FromLongLong(btni->y_end));

                PyTuple_SetItem(buttons, button, buttonPts);

                printf("Button %i top-left @ (%i,%i), bottom-right @ (%i,%i)\n",
                       button + 1, btni->x_start, btni->y_start,
                       btni->x_end, btni->y_end);
            }

            return buttons;

        }

        return PyTuple_New(0);

    }

/** DvdNav_push_button
 *
 * @param self
 * @param args
 * @return
 */
static PyObject *DvdNav_push_button(dvdNav *self, PyObject *args) {
    pci_t pci = self->dvdnav.getCurrentNavPCI();
    dsi_t dsi = self->dvdnav.getCurrentNavDSI();
    int btn;

    PyArg_ParseTuple(args, "i|", &btn);

    try {
        self->dvdnav.selectAndActivateButton(btn);
    } catch (const DvdNavException& e) {
        PyErr_SetString(PyExc_Exception, e.getMessage().c_str());
        return NULL;
    } catch (std::exception &e) {
        PyErr_SetString(PyExc_Exception, "Unknown Exception");
        return NULL;
    }

    Py_RETURN_NONE;
}

/** DvdNav_get_titles
 *
 * @param self
 * @param args
 * @return
 */
static PyObject *DvdNav_get_titles(dvdNav *self, PyObject *args) {
    try {
        return PyLong_FromLongLong(self->dvdnav.getTitles());
    } catch (const DvdNavException& e) {
        PyErr_SetString(PyExc_Exception, e.getMessage().c_str());
        return NULL;
    } catch (std::exception &e) {
        PyErr_SetString(PyExc_Exception, "Unknown Exception");
        return NULL;
    }

    Py_RETURN_NONE;
}

/** DvdNav_push_button
 *
 * @param self
 * @param args
 * @return
 */
static PyObject *DvdNav_get_parts(dvdNav *self, PyObject *args) {
    int title;

    PyArg_ParseTuple(args, "i|", &title);

    try {
        return PyLong_FromLongLong(self->dvdnav.getParts(title));
    } catch (const DvdNavException& e) {
        PyErr_SetString(PyExc_Exception, e.getMessage().c_str());
        return NULL;
    } catch (std::exception &e) {
        PyErr_SetString(PyExc_Exception, "Unknown Exception");
        return NULL;
    }

    Py_RETURN_NONE;
}

/** DvdNav_push_button
 *
 * @param self
 * @param args
 * @return
 */
static PyObject *DvdNav_get_angles(dvdNav *self, PyObject *args) {
    int title;

    PyArg_ParseTuple(args, "i|", &title);

    try {
        return PyLong_FromLongLong(self->dvdnav.getAngles(title));
    } catch (const DvdNavException& e) {
        PyErr_SetString(PyExc_Exception, e.getMessage().c_str());
        return NULL;
    } catch (std::exception &e) {
        PyErr_SetString(PyExc_Exception, "Unknown Exception");
        return NULL;
    }

    Py_RETURN_NONE;
}

    static PyMethodDef DvdNav_methods[] = {
            {"still_skip",                (PyCFunction) DvdNav_still_skip,                   METH_VARARGS, ""},
            {"wait_skip",                 (PyCFunction) DvdNav_wait_skip,                    METH_VARARGS, "If we are currently in WAIT state, that is: the application is required to wait for its fifos to become empty, calling this signals libdvdnav that this is achieved and that it can continue."},
            {"get_highlight",             (PyCFunction) DvdNav_get_highlight,                METH_VARARGS, ""},
            {"current_title_info",        (PyCFunction) DvdNav_current_title_info,           METH_VARARGS, ""},
            {"get_position",              (PyCFunction) DvdNav_get_position,                 METH_VARARGS, ""},
            {"get_current_nav_pci",       (PyCFunction) DvdNav_get_current_nav_buttons,      METH_VARARGS, ""},
            {"next_block",                (PyCFunction) DvdNav_next_block,                   METH_NOARGS,  "Return the name, combining the first and last name"},
            {"push_button",               (PyCFunction) DvdNav_push_button,                  METH_VARARGS, ""},
            {"get_current_title_program", (PyCFunction) DvdNav_dvdnav_current_title_program, METH_VARARGS, ""},
            {"get_number_of_titles",      (PyCFunction) DvdNav_get_titles,                   METH_VARARGS, ""},
            {"get_number_of_parts",      (PyCFunction) DvdNav_get_parts,                    METH_VARARGS, ""},
            {"get_number_of_angles",     (PyCFunction) DvdNav_get_angles,                   METH_VARARGS, ""},
            {NULL}  /* Sentinel */
    };

    static PyTypeObject DvdNavType = {
            .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
            .tp_name = "_pydvdnav.DvdNav",
            .tp_basicsize = sizeof(DvdNav),
            .tp_itemsize = 0,
            .tp_flags = Py_TPFLAGS_DEFAULT,
            .tp_doc = PyDoc_STR("DVDNAVTHINGY"),
            .tp_methods = DvdNav_methods,
            .tp_init = (initproc) DvdNav_init,
            .tp_new = PyType_GenericNew,
    };

    static PyModuleDef dvdnav_module = {.m_base = PyModuleDef_HEAD_INIT, .m_name = "dvdnav", .m_doc = "Example module that creates an extension type.", .m_size = -1,};

    PyMODINIT_FUNC PyInit__pydvdnav(void) {
        PyObject *m;
        if (PyType_Ready(&DvdNavType) < 0)
            return NULL;

        m = PyModule_Create(&dvdnav_module);
        if (m == NULL)
            return NULL;

        if (PyModule_AddObject(m, "DvdNav", (PyObject *) &DvdNavType) < 0) {
            Py_DECREF(&DvdNavType);
            Py_DECREF(m);
            return NULL;
        }

        return m;
    }
}