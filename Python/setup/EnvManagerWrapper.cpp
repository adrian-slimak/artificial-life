#include <Python.h>
#include <cstdio>
//~ #include "structmember.h"
#include <iostream>

#include "../../Simulation/Simulation/EnvManager.h"

typedef struct {
    PyObject_HEAD
    EnvManager * ptrObj;
} PyEnvManager;

static int PyEnvManager_init(PyEnvManager *self, PyObject *args, PyObject *kwds)
// initialize EnvManager Object
{
//    int fftSize;
//
//    if (! PyArg_ParseTuple(args, "i", &fftSize))
//        return -1;

    self->ptrObj = new EnvManager();

    return 0;
}

static void PyEnvManager_dealloc(PyEnvManager* self)
// destruct the object
{
    delete self->ptrObj;
    Py_TYPE(self)->tp_free(self);
}


static PyObject * PyEnvManager_run_single_episode(PyEnvManager* self, PyObject* args)
{
    (self->ptrObj)->run_single_episode();
    return Py_None;
}

static PyObject * PyEnvManager_set_prey_genes(PyEnvManager* self, PyObject* args)
{
    double pointer_as_double;
    int genes_count;
    int genes_length;

    if (!PyArg_ParseTuple(args, "dii", &pointer_as_double, &genes_count, &genes_length))
        return Py_False;

    (self->ptrObj)->set_prey_genes((float*)((long long int) pointer_as_double), genes_count, genes_length);
    return Py_None;
}

static PyObject * PyEnvManager_set_predator_genes(PyEnvManager* self, PyObject* args)
{
    double pointer_as_double;
    int genes_count;
    int genes_length;

    if (!PyArg_ParseTuple(args, "dii", &pointer_as_double, &genes_count, &genes_length))
        return Py_False;

    (self->ptrObj)->set_predator_genes((float*)((long long int) pointer_as_double), genes_count, genes_length);
    return Py_None;
}

static PyObject * PyEnvManager_set_fitness_pointers(PyEnvManager* self, PyObject* args)
{
    double prey_pointer;
    double predator_pointer;

    if (!PyArg_ParseTuple(args, "dd", &prey_pointer, &predator_pointer))
        return Py_False;

    (self->ptrObj)->set_fitness_pointers((float*)((long long int) prey_pointer), (float*)((long long int) predator_pointer));
    return Py_None;
}

static PyObject * PyEnvManager_set_stats_pointers(PyEnvManager* self, PyObject* args)
{
    double prey_pointer;
    double predator_pointer;

    if (!PyArg_ParseTuple(args, "dd", &prey_pointer, &predator_pointer))
        return Py_False;

    (self->ptrObj)->set_stats_pointers((float*)((long long int) prey_pointer), (float*)((long long int) predator_pointer));
    return Py_None;
}

// EnvManager object member methods
static PyMethodDef PyEnvManager_methods[] = {
    { "run_single_episode", (PyCFunction)PyEnvManager_run_single_episode, METH_VARARGS, "Run single episode" },
    { "set_prey_genes", (PyCFunction)PyEnvManager_set_prey_genes, METH_VARARGS, "Set weights (Genes) for preys models" },
    { "set_predator_genes", (PyCFunction)PyEnvManager_set_predator_genes, METH_VARARGS, "Set weights (Genes) for predators models" },
    { "set_fitness_pointers", (PyCFunction)PyEnvManager_set_fitness_pointers, METH_VARARGS, "Set pointers to arrays to store fitness of models (Genes)" },
    { "set_stats_pointers", (PyCFunction)PyEnvManager_set_stats_pointers, METH_VARARGS, "Set pointers to arrays to store fitness of models (Genes)" },
//    { "set_parameters", (PyCFunction)EnvManager_set_parameters, METH_VARARGS, "Set environment parameters" },
    {NULL, NULL, NULL}  /* Sentinel */
};


// static method for environment parameters initialization
static PyObject * envmanager_set_parameters(PyObject* self, PyObject* args)
{
    const char *params_path;
    Py_ssize_t size;

    if (!PyArg_ParseTuple(args, "s#", &params_path, &size))
        return Py_False;

    EnvManager::set_parameters(params_path);
    return Py_None;
}

// envmanader module static methods
static PyMethodDef envmanager_methods[] = {
	{"set_parameters", envmanager_set_parameters, METH_VARARGS, "Set environment parameters from selected file"},
	{NULL, NULL}		/* sentinel */
};

static PyModuleDef envmanagermodule = {
    PyModuleDef_HEAD_INIT,
    "envmanager",
    "Example module that wrapped a C++ object",
    -1,
    envmanager_methods
};

static PyTypeObject PyEnvManagerType = { PyVarObject_HEAD_INIT(NULL, 0)
                                    "envmanager.EnvManager"   /* tp_name */
                                    };

PyMODINIT_FUNC PyInit_EnvManager(void)
// create the module
{
    PyObject* m;

    PyEnvManagerType.tp_new = PyType_GenericNew;
    PyEnvManagerType.tp_basicsize=sizeof(PyEnvManager);
    PyEnvManagerType.tp_dealloc=(destructor) PyEnvManager_dealloc;
    PyEnvManagerType.tp_flags=Py_TPFLAGS_DEFAULT;
    PyEnvManagerType.tp_doc="EnvManager objects";
    PyEnvManagerType.tp_methods=PyEnvManager_methods;
    //~ PyVoiceType.tp_members=Noddy_members;
    PyEnvManagerType.tp_init=(initproc)PyEnvManager_init;

    if (PyType_Ready(&PyEnvManagerType) < 0)
        return NULL;

    m = PyModule_Create(&envmanagermodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&PyEnvManagerType);
    PyModule_AddObject(m, "EnvManager", (PyObject *)&PyEnvManagerType); // Add EnvManager object to the module
    return m;
}