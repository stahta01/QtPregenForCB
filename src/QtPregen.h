/***************************************************************
 * Name:      QtPregen.h
 * Purpose:   Code::Blocks plugin 'qtPregenForCB.cbp'   0.2.2
 * Author:    LETARTARE
 * Created:   2015-02-09
 * Copyright: LETARTARE
 * License:   GPL
 **************************************************************/

//-------------------------------------------------
#ifndef _QTPREGEN_H_
#define _QTPREGEN_H_
//-------------------------------------------------
// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>
//-------------------------------------------------
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
// for "class cbPlugin"  : reference SDK
#include <cbplugin.h>
//-------------------------------------------------
class qtPre;
class qtPrebuild;

/** @brief Qt plugin main class
 *
*/
class QtPregen : public cbPlugin
{
  public:
    /** Constructor */
    QtPregen();

    /** Destructor      */
    virtual ~QtPregen() {}

    /** Invoke configuration dialog. */
    virtual int Configure() {return 0;}

    /** Return the plugin's configuration priority.
      * This is a number (default is 50) that is used to sort plugins
      * in configuration dialogs. Lower numbers mean the plugin's
      * configuration is put higher in the list.
      */
    virtual int GetConfigurationPriority() const { return 50; }

    /** Return the configuration group for this plugin. Default is cgUnknown.
      * Notice that you can logically OR more than one configuration groups,
      * so you could set it, for example, as "cgCompiler | cgContribPlugin".
      */
    virtual int GetConfigurationGroup() const { return cgContribPlugin; }

    /** Return plugin's configuration panel.
      * @param parent The parent window.
      * @return A pointer to the plugin's cbConfigurationPanel. It is deleted by the caller.
      */
    virtual cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent) { return 0 ;}

    /** Return plugin's configuration panel for projects.
      * The panel returned from this function will be added in the project's
      * configuration dialog.
      * @param parent The parent window.
      * @param project The project that is being edited.
      * @return A pointer to the plugin's cbConfigurationPanel. It is deleted by the caller.
      */
    virtual cbConfigurationPanel* GetProjectConfigurationPanel(wxWindow* parent, cbProject* project){ return 0 ;}

    /** This method is called by Code::Blocks and is used by the plugin
      * to add any menu items it needs on Code::Blocks's menu bar.\n
      * It is a pure virtual method that needs to be implemented by all
      * plugins. If the plugin does not need to add items on the menu,
      * just do nothing ;)
      * @param menuBar the wxMenuBar to create items in
      */
    virtual void BuildMenu(wxMenuBar* menuBar);

    /** This method is called by Code::Blocks core modules (EditorManager,
      * ProjectManager etc) and is used by the plugin to add any menu
      * items it needs in the module's popup menu. For example, when
      * the user right-clicks on a project file in the project tree,
      * ProjectManager prepares a popup menu to display with context
      * sensitive options for that file. Before it displays this popup
      * menu, it asks all attached plugins (by asking PluginManager to call
      * this method), if they need to add any entries
      * in that menu. This method is called.\n
      * If the plugin does not need to add items in the menu,
      * just do nothing ;)
      * @param type the module that's preparing a popup menu
      * @param menu pointer to the popup menu
      * @param data pointer to FileTreeData object (to access/modify the file tree)
      */
    virtual void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0);

    /** This method is called by Code::Blocks and is used by the plugin
      * to add any toolbar items it needs on Code::Blocks's toolbar.\n
      * It is a pure virtual method that needs to be implemented by all
      * plugins. If the plugin does not need to add items on the toolbar,
      * just do nothing ;)
      * @param toolBar the wxToolBar to create items on
      * @return The plugin should return true if it needed the toolbar, false if not
      */
    virtual bool BuildToolBar(wxToolBar* toolBar) { return false ;}

     /** @brief Execute the plugin.
      *
      * This is the only function needed by a cbToolPlugin.
      * This will be called when the user selects the plugin from the "Plugins"
      * menu.
      */
    virtual int Execute() { return 0 ;}

  protected:
    /** Any descendent plugin should override this virtual method and
      * perform any necessary initialization. This method is called by
      * Code::Blocks (PluginManager actually) when the plugin has been
      * loaded and should attach in Code::Blocks. When Code::Blocks
      * starts up, it finds and <em>loads</em> all plugins but <em>does
      * not</em> activate (attaches) them. It then activates all plugins
      * that the user has selected to be activated on start-up.\n
      * This means that a plugin might be loaded but <b>not</b> activated...\n
      * Think of this method as the actual constructor...
      */
    virtual void OnAttach();

    /** Any descendent plugin should override this virtual method and
      * perform any necessary de-initialization. This method is called by
      * Code::Blocks (PluginManager actually) when the plugin has been
      * loaded, attached and should de-attach from Code::Blocks.\n
      * Think of this method as the actual destructor...
      * @param appShutDown If true, the application is shutting down. In this
      *         case *don't* use Manager::Get()->Get...() functions or the
      *         behaviour is undefined...
      */
    virtual void OnRelease(bool appShutDown);

    /** This method called by 'Build' allows pre-build all the additional
      * files required to compile a project using the Qt libraries
      * @param event Contains the event which call this method
      */
    void OnPrebuild(CodeBlocksEvent& event);

  private:
    /** project
      */
    cbProject* project;
    /** pre-Builder for QT
    */
    qtPrebuild * prebuild;
    /** Indicates that everything is properly pre-built
    */
    bool buildAllOk;
    /** Indicates that everything is properly pre-clean
    */
    bool cleanAllOk;
    /**  Id menus
    */
    int16_t IdBuild, IdCompile, IdRun, IdBuildRun, IdRebuild, IdClean;
    /** Id popup on project
    */
    int16_t IdpBuild, IdpRebuild, IdpClean;
    /** Id popup on file
    */
    int16_t IdfBuild, IdfClean ;
    /** Global used to record different messages for the construction report
    */
    wxString Mes;
};

#endif // _CBQTPREGEN_H_
//-------------------------------------------------