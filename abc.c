/************************* Compile using:************************************************* */
//gcc -Wall -g -o test abc.c -export-dynamic `pkg-config gtk+-2.0 libglade-2.0 --cflags --libs`      


#include <gtk/gtk.h>


typedef struct {
        GtkWidget               *window;
        GtkWidget               *statusbar;
        GtkWidget             *textview1;
        GtkWidget             *textview2;
	GtkButton               button1;
	GtkButton               button2;
	GtkButton               button3;
	GtkButton               button4;
        guint                   statusbar_context_id;
	gchar                   *filename;
} gui_struct;

/* location of UI XML file relative to path in which program is running */
 void on_window_destroy (GtkObject *object, gpointer user_data){
	printf("rohit: window is closed\n");
	gtk_main_quit();
}

void on_drscheme_clicked(GtkButton *object,gui_struct *editor);
void on_topython_clicked(GtkButton *object,gui_struct *editor);

void on_expython_clicked(GtkButton *object,gui_struct *editor);
void on_android_clicked(GtkButton *object,gui_struct *editor);

void on_drscheme_clicked(GtkButton *button1,gui_struct *gui){
	GtkTextBuffer  *buffer1;
	GtkTextIter     start,end;
	GError         *err=NULL;
	gchar          *text;
	gboolean       result;
	printf("dsrscheme clicked\n");

//saving the content of the scheme section to in.sch 
//1.copying the content of buffer to text
	gtk_widget_set_sensitive (gui->textview1, FALSE);
       	buffer1 = gtk_text_view_get_buffer (GTK_TEXT_VIEW (gui->textview1));
	gtk_text_buffer_get_start_iter(buffer1,&start);
	gtk_text_buffer_get_end_iter(buffer1,&end);
	text = gtk_text_buffer_get_text(buffer1,&start,&end,FALSE);
	gtk_widget_set_sensitive (gui->textview1, TRUE);
	result = g_file_set_contents("in.sch" , text ,-1 , &err);
	
	if(fork()==0)
		system("drscheme in.sch");

}

void on_topython_clicked(GtkButton *button1,gui_struct *gui){
	GtkTextBuffer  *buffer1;
	GtkTextBuffer  *buffer2;
	GtkTextIter     start,end;
	GError         *err=NULL;
	gchar          *text;
	gboolean       result;

	printf("to-python clicked\n");


//saving the content of the scheme section to in.sch 
//1.copying the content of buffer to text
	gtk_widget_set_sensitive (gui->textview1, FALSE);
       	buffer1 = gtk_text_view_get_buffer (GTK_TEXT_VIEW (gui->textview1));
	gtk_text_buffer_get_start_iter(buffer1,&start);
	gtk_text_buffer_get_end_iter(buffer1,&end);
	text = gtk_text_buffer_get_text(buffer1,&start,&end,FALSE);
	gtk_widget_set_sensitive (gui->textview1, TRUE);
//2 writitn text to a file in.sch
	result = g_file_set_contents("in.sch" , text ,-1 , &err);

	if(result==FALSE)
		printf("file not created \n");	
//exectuting the executable sch2py  
//to generate out.py 
//from the input file in.sch
	system("./mce < in.sch >out.py");


//displaying the contents of file "out.py" to python section 	
	result = g_file_get_contents ("out.py", &text, NULL, &err);
	gtk_widget_set_sensitive (gui->textview2, FALSE);
        buffer2 = gtk_text_view_get_buffer (GTK_TEXT_VIEW (gui->textview2));
        gtk_text_buffer_set_text (buffer2, text, -1);

}


void on_expython_clicked(GtkButton *button1,gui_struct *editor){
	printf("ex-python clicked\n");
	if(fork()==0)
		system("idle out.py");

}
void on_android_clicked(GtkButton *button1,gui_struct *editor){
	printf("android clicked\n");
	if(fork()==0)
		system("./script");
}





//when help -> about clicked
void on_about_menu_item_activate (GtkMenuItem *menuitem,gui_struct  *editor)
{
	static const gchar * const authors[] = {"Rohit Nair<email :  rohitnjan88@gmail.com>",NULL};
	static const gchar copyright[] = "Copyright \xc2\xa9 2010 Rohit Nair";
	static const gchar comments[] = "Scheme translated to corresponding Python Code";
	gtk_show_about_dialog (GTK_WINDOW (editor->window),
				"authors", authors,"comments", comments,
				"copyright", copyright,
				"version", "0.1",
				"website", "http:www.nairohit.blogspot.com",
				"program-name", "Scheme-to-Python Translator",
				"logo-icon-name", GTK_STOCK_EDIT,NULL); 
}






//-----------------------------------------------------
//-----------------------------------------------------
void
error_message (const gchar *message)
{
	GtkWidget               *dialog;
	/* log to terminal window */
	g_warning (message);
	/* create an error message dialog and display modally to the user */
	dialog = gtk_message_dialog_new (NULL, 
		GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_ERROR,
		GTK_BUTTONS_OK,
		message);
	gtk_window_set_title (GTK_WINDOW (dialog), "Error!");
	gtk_dialog_run (GTK_DIALOG (dialog));      
	gtk_widget_destroy (dialog);         
}




gchar *
get_open_filename (gui_struct *editor)
{
        GtkWidget               *chooser;
        gchar                   *filename=NULL;
	chooser = gtk_file_chooser_dialog_new ("Open File...",
			GTK_WINDOW (editor->window),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_OK,
			NULL);
	if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_OK){
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
	}
	gtk_widget_destroy (chooser);
	return filename;
}

get_save_filename (gui_struct *editor)
{
	GtkWidget               *chooser;
	gchar                   *filename=NULL;
	chooser = gtk_file_chooser_dialog_new ("Save File...",
						GTK_WINDOW (editor->window),
						GTK_FILE_CHOOSER_ACTION_SAVE,
						GTK_STOCK_CANCEL, 
						GTK_RESPONSE_CANCEL,
						GTK_STOCK_SAVE, 
						GTK_RESPONSE_OK,
						NULL);
	if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_OK){
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
	}
	gtk_widget_destroy (chooser);
	return filename;
}

/* 
   We call load_file() when we have a filename and want to load it into the buffer
   for the GtkTextView. The previous contents are overwritten.
   */
void 
load_file (gui_struct *editor, gchar *filename)
{
	GError                  *err=NULL;
	gchar                   *status;
	gchar                   *text;
	gboolean                result;
	GtkTextBuffer           *buffer;
						        
	/* add Loading message to status bar and  ensure GUI is current */
/*	status = g_strdup_printf ("Loading %s...", filename);
	gtk_statusbar_push (GTK_STATUSBAR (editor->statusbar),
	editor->statusbar_context_id, status);
	g_free (status);
	while (gtk_events_pending()) gtk_main_iteration();
	*/


	/* get the file contents */
	result = g_file_get_contents (filename, &text, NULL, &err);
	if (result == FALSE){
		/* error loading file, show message to user */
		error_message (err->message);
		g_error_free (err);
		g_free (filename);
	}
	
	/* disable the text view while loading the buffer with the text */    
	gtk_widget_set_sensitive (editor->textview1, FALSE);
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (editor->textview1));
	gtk_text_buffer_set_text (buffer, text, -1);
	gtk_text_buffer_set_modified (buffer, FALSE);
	gtk_widget_set_sensitive (editor->textview1, TRUE);
	g_free (text); 
	
	/* now we can set the current filename since loading was a success */
	if (editor->filename != NULL) g_free (editor->filename);
	editor->filename = filename;
	
	/* clear loading status and restore default  */
//	gtk_statusbar_pop (GTK_STATUSBAR (editor->statusbar),editor->statusbar_context_id);
//	reset_default_status (editor);
}


/* 
   We call write_file() when we have a filename and want to load the current text
   buffer for the GtkTextView into the file.
   */
void 
write_file (gui_struct *editor, gchar *filename)
{
        GError                  *err=NULL;
        gchar                   *status;
        gchar                   *text;
        gboolean                result;
        GtkTextBuffer           *buffer;
        GtkTextIter             start, end;
        
       /* add Saving message to status bar and ensure GUI is current */
       /* if (filename != NULL)
           status = g_strdup_printf ("Saving %s...", filename);
        else
           status = g_strdup_printf ("Saving %s...", editor->filename);
									            
	gtk_statusbar_push (GTK_STATUSBAR (editor->statusbar),
	editor->statusbar_context_id, status);
	g_free (status);
	while (gtk_events_pending()) gtk_main_iteration();
	*/							        
	/* disable text view and get contents of buffer */ 
	gtk_widget_set_sensitive (editor->textview1, FALSE);
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (editor->textview1));
	gtk_text_buffer_get_start_iter (buffer, &start);
	gtk_text_buffer_get_end_iter (buffer, &end);
	text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);       
	gtk_text_buffer_set_modified (buffer, FALSE);
	gtk_widget_set_sensitive (editor->textview1, TRUE);
														        
														        /* set the contents of the file to the text from the buffer */
	if (filename != NULL)        
		result = g_file_set_contents (filename, text, -1, &err);
	else
		result = g_file_set_contents (editor->filename, text, -1, &err);
	if (result == FALSE){
	/* error saving file, show message to user */
		error_message (err->message);
		g_error_free (err);
	}        
	/* don't forget to free that memory! */ 
	g_free (text); 

	if (filename != NULL){
		/* we need to free the memory used by editor->filename and set 
	it to the new filename instead */
		if (editor->filename != NULL) g_free (editor->filename);
		editor->filename = filename;
	}
	/* clear saving status and restore default */
//	gtk_statusbar_pop (GTK_STATUSBAR (editor->statusbar),editor->statusbar_context_id);
//	reset_default_status (editor);   
}



gboolean
check_for_save (gui_struct *editor)
{
	gboolean                ret = FALSE;
	GtkTextBuffer           *buffer;
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (editor->textview1));
	if (gtk_text_buffer_get_modified (buffer) == TRUE){
		/* we need to prompt for save */
		GtkWidget       *dialog;
		const gchar *msg  = "Do you want to save the changes you have made?";
		dialog = gtk_message_dialog_new (NULL, 
						GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_MESSAGE_QUESTION,
						GTK_BUTTONS_YES_NO,
						msg);
		gtk_window_set_title (GTK_WINDOW (dialog), "Save?");
		if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_NO){
			ret = FALSE;
		}      
		else ret = TRUE;
		gtk_widget_destroy (dialog);      
	}     
	return ret;
}





/*
   Called when the user clicks the 'New' menu. We need to prompt for save if the
   file has been modified, and then delete the buffer and clear the modified flag.
   */
void on_new_menu_item_activate (GtkMenuItem *menuitem, gui_struct *editor)
{
	GtkTextBuffer           *buffer;
	if (check_for_save (editor) == TRUE){
		on_save_menu_item_activate (NULL, editor);  
	}
	/* clear editor for a new file */
	editor->filename = NULL;
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (editor->textview1));
	gtk_text_buffer_set_text (buffer, "", -1);
	gtk_text_buffer_set_modified (buffer, FALSE);
}


/*
   Called when the user clicks the 'Save' menu. We need to allow the user to choose 
   a file to save if it's an untitled document, and then call write_file() on that 
   file.
   */
void on_save_menu_item_activate (GtkMenuItem *menuitem,gui_struct *editor)
{
	gchar *filename;
	if (editor->filename == NULL) {
		filename = get_save_filename (editor);
		if (filename != NULL) write_file (editor, filename); 
	}
	else 
		write_file (editor, NULL);			        
}


/*
   Called when the user clicks the 'Open' menu. We need to prompt for save if the
   file has been modified, allow the user to choose a file to open, and then call
   load_file() on that file.
   */
void on_open_menu_item_activate (GtkMenuItem *menuitem, gui_struct *editor)
{
	gchar                   *filename;
	if (check_for_save (editor) == TRUE){
		on_save_menu_item_activate (NULL, editor);  
	}
	filename = get_open_filename (editor);
	if (filename != NULL) load_file (editor, filename); 
}




/*
   Called when the user clicks the 'Save As' menu. We need to allow the user to 
   choose a file to save and then call write_file() on that file.
   */
void on_save_as_menu_item_activate (GtkMenuItem *menuitem, gui_struct *editor)
{
	 gchar *filename;
	 filename = get_save_filename (editor);
	 if (filename != NULL) 
		 write_file (editor, filename); 
}



void on_quit_menu_item_activate (GtkMenuItem *menuitem, gui_struct *editor)
{
	        if (check_for_save (editor) == TRUE){
			on_save_menu_item_activate (NULL, editor);  
		}
		gtk_main_quit();
}










/*
   Called when the user clicks the 'Cut' menu. 
   */
void on_cut_menu_item_activate (GtkMenuItem *menuitem,gui_struct *editor)
{
	GtkTextBuffer           *buffer;
	GtkClipboard            *clipboard;
	clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (editor->textview1));
	gtk_text_buffer_cut_clipboard (buffer, clipboard, TRUE);
}

/*
   Called when the user clicks the 'Copy' menu. 
   */
void on_copy_menu_item_activate (GtkMenuItem *menuitem, gui_struct *editor)
{
	GtkTextBuffer           *buffer;
	GtkClipboard            *clipboard;
			        
	clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (editor->textview1));
	gtk_text_buffer_copy_clipboard (buffer, clipboard);
}

/*
   Called when the user clicks the 'Paste' menu. 
   */
void on_paste_menu_item_activate (GtkMenuItem *menuitem,gui_struct *editor)
{
	GtkTextBuffer           *buffer;
	GtkClipboard            *clipboard;
	clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (editor->textview1));
	gtk_text_buffer_paste_clipboard (buffer, clipboard, NULL, TRUE);
}

/*
   Called when the user clicks the 'Delete' menu. 
   */
void on_delete_menu_item_activate (GtkMenuItem *menuitem, gui_struct *editor)
{
	GtkTextBuffer           *buffer;
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (editor->textview1));
	gtk_text_buffer_delete_selection (buffer, FALSE, TRUE);
}

//--------------------------------------------------------
//--------------------------------------------------------




main (int argc, char *argv[])
{
	gui_struct *gui;
	gui = g_slice_new(gui_struct);

	GtkBuilder *builder;
	gtk_init (&argc, &argv);

	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder,"abc.xml",NULL);
        
//
	gui->window = GTK_WIDGET (gtk_builder_get_object (builder, "window"));
	gui->textview1 = GTK_WIDGET (gtk_builder_get_object (builder, "textview1"));
	gui->textview2 = GTK_WIDGET (gtk_builder_get_object (builder, "textview2"));
 


//connecting signals  
	gtk_builder_connect_signals (builder, gui);
        g_object_unref (G_OBJECT (builder));

        gtk_widget_show (gui->window);
        gtk_main ();
	g_slice_free(gui_struct,gui);
        return 0; 
}



