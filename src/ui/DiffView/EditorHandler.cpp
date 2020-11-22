#include "EditorHandler.h"
#include "Editor.h"

#include <QDebug>
#include <QTimer>

EditorHandler* EditorHandler::_instance = nullptr; /* static elements of a class must be initialized */

EditorHandler::EditorHandler (): mTimer(new QTimer(this)) {}

Editor* EditorHandler::getEditor(QWidget* newParent) {
    int max_size = 0;
    for (int i=0; i < mEditors.length(); i++) {
        int size = sizeof (*mEditors[i].editor);
        if (size > max_size)
            max_size = size;
    }
    // return next free editor
    //for (EditorFree e: mEditors) {} // not possible, because it creates a copy of EditorFree object, which is not possible, because then a copy of the unique_ptr is made which is not allowed
    for (int i=0; i < mEditors.length(); i++) {
        if (!mEditors[i].occupied) { // EditorHandler::EditorFree::EditorFree(const EditorHandler::EditorFree&) // why this constructor is needed?
            mEditors[i].occupied = true;
            mEditors[i].editor->setParent(newParent);
            return mEditors[i].editor; //.get();
        }
    }
    // no free editor. So lets create a new instance
    mEditors.append(EditorFree());

    mEditors.last().occupied = true;
    mEditors.last().editor = new Editor(newParent); // std::unique_ptr<Editor>(new Editor(nullptr));
    return mEditors.last().editor; // mEditors.last().editor.get();
}

void EditorHandler::releaseEditor(TextEditor* editor) {
    for (int i=0; i < mEditors.length(); i++) {
        if (mEditors[i].editor == editor) {
            mEditors[i].editor->setParent(nullptr); // otherwise the object gets deleted when the parent gets deleted
            mEditors[i].occupied = false;
            // Check all 5 minutes if unneccessary editors exist and delete them
            mTimer->stop();
            QTimer::singleShot(5*60*1000, this, &EditorHandler::deleteEditors);
            return;
        }
    }
}

void EditorHandler::deleteEditors() {
    for (int i= 0; i < mEditors.length(); i++) {
        if (!mEditors[i].occupied) {
            connect(mEditors[i].editor, &TextEditor::destroyed, this, &EditorHandler::editorDestroyed);
            delete mEditors[i].editor;
        }
    }
}

void EditorHandler::editorDestroyed(QObject* e) {
    for (int i = 0; i < mEditors.length(); i++) {
        if (mEditors[i].editor == e) {
            mEditors.remove(i);
        }
    }
}
