#ifndef FILEDIALOG_HPP_INCLUDED
#define FILEDIALOG_HPP_INCLUDED
#include "Dialog.hpp"

class FileDialog : public Dialog
{
public:
	FileDialog(EditorState *mstate, FileParserType type,
		const wchar_t* title, const wchar_t* button);
	static bool show(EditorState *mstate, FileParserType type,
			const wchar_t* title, const wchar_t* button);
	void doSave(const SEvent &event);
	void doOpen(const SEvent &event);
	virtual bool canClose();
	virtual bool close();
	virtual bool OnEvent(const SEvent &event);
private:
	FileParserType parser_type;
	IGUIWindow *win;
};

#endif

