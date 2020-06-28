#pragma once
#pragma unmanaged

#include "properties.h"
#include "zip.h"
#include "unzip.h"
#pragma unmanaged
void RenderLoop();

void SaveState(char* name);
void LoadTextures(register const BYTE const* __restrict pFile);

void ReleaseTextures();

__declspec(noalias) void DrawFrame();


/*static bool render = false;
bool showTextures = true;
bool showSky = true;
bool enableTransparency = true;
bool showRails = false;
bool showSpawns = false;
bool showFlags = false;
bool update = true;*/

extern bool showTextures;
extern bool showSky;
extern bool enableTransparency;
extern bool showRails;
extern bool showSpawns;
extern bool showFlags;
extern bool update;
extern bool disableKeys;
extern bool FPS;

/*void Renderer()
{
  while(true)
  {
    if(render)
    {
      UpdateAndRender();
    }
    else
      Update();
  }
}*/
#pragma managed




/*static short control_word;
static short control_word2;

//void DrawFrame();

static inline void SetFloatingPointRoundingToTruncate()
{
__asm
{
fstcw   control_word                // store fpu control word
mov     dx, word ptr [control_word]
or      dx, 0x0C00                  // rounding: truncate
mov     control_word2, dx
fldcw   control_word2               // load modfied control word
}
}*/
extern HWND hEdit;

namespace THPSLevelEditor {

  using namespace System;
  using namespace System::ComponentModel;
  using namespace System::Collections;
  using namespace System::Windows::Forms;
  using namespace System::Data;
  using namespace System::Drawing;

  //public ref class RenderPanel : public System::Windows::Forms::Panel
  //{
  //protected:
  //  virtual void OnPaint( PaintEventArgs^ e ) override
  //  {
  //MessageBox::Show("Panel::OnPaint");
  //  }
  //  virtual void OnPaintBackground( PaintEventArgs^ e ) override
  //  {
  //MessageBox::Show("Panel::OnPaintBackGround");
  //  }
  //};

  /// <summary>
  /// Summary for THPSLevel
  /// </summary>
  public ref class THPSLevel : public System::Windows::Forms::Form
  {
  public: static System::Windows::Forms::ListBox^  listBox1;
  public: static System::Windows::Forms::ToolStripProgressBar^  toolStripProgressBar1;
  public: static System::Windows::Forms::ToolStripStatusLabel^  toolStripStatusLabel1;
  public: static System::Windows::Forms::ListBox^  listBox2;
  public: static System::Windows::Forms::ToolStripButton^  toolStripButton3;
  public: static System::Windows::Forms::ToolStripButton^  toolStripButton4;
  public: static System::Windows::Forms::TreeView^  MaterialList;



  protected: bool ProcessCmdKey(System::Windows::Forms::Message% msg, System::Windows::Forms::Keys keyData) override
  {
				 if (disableKeys)
				 {
					 SetFocus(hEdit);
					 keyData = Keys::None;
				 }
				 return (__super::ProcessCmdKey(msg, keyData));
  }











































  public: static System::Collections::Generic::List<MeshProperties^>^ properties;
          //public: static DWORD selectedMeshIndex;

          //static bool rendering = false;







          /* protected: virtual void OnPaintBackGround(PaintEventArgs^ e) override
          {
          this->properties = gcnew System::Collections::Generic::List<MeshProperties^>();
          }
          protected: virtual void OnPaint(PaintEventArgs^ e) override
          {
          }*/








  public: System::Windows::Forms::ToolStripStatusLabel^  toolStripStatusLabel2;
  public: static System::Windows::Forms::ComboBox^ comboBox2;
  public: static System::Windows::Forms::TextBox^ codeTextBox;






          //private: System::Windows::Forms::Timer^  timer1;
  private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;
  private: System::Windows::Forms::ToolStripButton^  toolStripButton7;
  private: System::Windows::Forms::ToolStripButton^  toolStripButton6;
  private: System::Windows::Forms::Panel^  panel7;
  private: System::Windows::Forms::ToolStripMenuItem^  materialToolStripMenuItem;
  private: System::Windows::Forms::TabPage^  tabPage4;
  private: System::Windows::Forms::TextBox^  draworder;
  private: System::Windows::Forms::GroupBox^  animgroup;
  private: System::Windows::Forms::GroupBox^  groupBox3;
  private: System::Windows::Forms::RadioButton^  water;
  private: System::Windows::Forms::RadioButton^  linear;
  private: System::Windows::Forms::Label^  label17;
  private: System::Windows::Forms::TextBox^  vvel;
  private: System::Windows::Forms::Label^  label16;
  private: System::Windows::Forms::Label^  label15;
  private: System::Windows::Forms::TextBox^  uvel;
  private: System::Windows::Forms::CheckBox^  animated;
  private: System::Windows::Forms::Button^  BrowseTexture;
  private: 

  private: 

  private: System::Windows::Forms::CheckBox^  doubled;
  private: System::Windows::Forms::Label^  label14;
  private: System::Windows::Forms::CheckBox^  transparent;
  private: System::Windows::Forms::ToolStripButton^  toolStripButton8;
private: System::Windows::Forms::Panel^  panel9;
private: System::Windows::Forms::Panel^  pictureBox1;

  public: 
    //static int tLast = 0;
    //static bool render = false;
    String^ sceneName;


    THPSLevel(void)
    {
      InitializeComponent();
      //
      //TODO: Add the constructor code here
      //

      this->Show();
      panel6->Focus();
      /*CreateThread(0,0,(LPTHREAD_START_ROUTINE)Renderer,0,0,0);
      Application::Run();*/


      while(Created)
      {
        if (FPS)
          UpdateFPS();
        else
          UpdateAndRender();

        Application::DoEvents();
      }
    }

  protected:
    /// <summary>
    /// Clean up any resources being used.
    /// </summary>

    ~THPSLevel()
    {
      if (components)
      {
        delete components;
      }
      CleanupAppdata();
    }

  private:
    /// <summary>
    /// Required designer variable.
    /// </summary>

    void AttemptRecovery();

    //void DrawFrame(/*System::Object ^,System::EventArgs ^*/);

    void InitRenderScene();

    void SetDeviceStates();

    void CreateManagedObjects();

    void DeleteManagedObjects();

    void SafeTurnOffRendering();

    bool LoadScene(char* path);

    bool LoadState(char* name);

    void OpenLoadSceneDialog(const char* const filter);


    void UnloadScene();
    void CleanupAppdata();


    System::Windows::Forms::MenuStrip^ menuStrip1;
    System::Windows::Forms::ToolStripMenuItem^ fileToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ importToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ tHPS2ToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ tHPS3ToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ tHPS4ToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ exportToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ loadToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ saveToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ exitToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ helpToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ tHPS3ToolStripMenuItem1;
    System::Windows::Forms::ToolStripMenuItem^ tHUG2ToolStripMenuItem1;
    System::Windows::Forms::ToolStripMenuItem^ oBJToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ aboutToolStripMenuItem;
    System::Windows::Forms::ToolStrip^ toolStrip1;
    System::Windows::Forms::StatusStrip^ statusStrip1;
    System::Windows::Forms::SplitContainer^ splitContainer1;
    System::Windows::Forms::ToolStripSplitButton^ toolStripSplitButton1;
    System::Windows::Forms::ToolStripMenuItem^ objectModeToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ surfaceModeToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ railModeToolStripMenuItem;
    System::Windows::Forms::ToolStripSeparator^ toolStripSeparator1;
    System::Windows::Forms::SplitContainer^ splitContainer2;
    System::Windows::Forms::Label^ label1;

    System::Windows::Forms::TabControl^ tabControl1;
    System::Windows::Forms::TabPage^ tabPage1;
    System::Windows::Forms::PropertyGrid^ propertyGrid1;
    System::Windows::Forms::TabPage^ tabPage2;
    System::Windows::Forms::Label^ label2;
    System::Windows::Forms::TabPage^ tabPage3;
    System::Windows::Forms::Panel^ panel1;
    System::Windows::Forms::Button^ button2;
    System::Windows::Forms::Button^ button1;
    System::Windows::Forms::Panel^ panel3;



    System::Windows::Forms::Label^ label7;
    System::Windows::Forms::TextBox^ textBox3;
    System::Windows::Forms::Label^ label6;
    System::Windows::Forms::TextBox^ textBox2;
    System::Windows::Forms::Label^ label5;
    System::Windows::Forms::TextBox^ textBox1;
    System::Windows::Forms::TrackBar^ trackBar1;
    System::Windows::Forms::Label^ label4;
    System::Windows::Forms::ComboBox^ comboBox1;
    System::Windows::Forms::Label^ label8;
    System::Windows::Forms::Button^ button4;
    System::Windows::Forms::Button^ button3;

    System::Windows::Forms::Panel^ panel4;
    System::Windows::Forms::Label^ label9;


    System::Windows::Forms::GroupBox^ groupBox1;
    System::Windows::Forms::ToolStripDropDownButton^ toolStripDropDownButton1;
    System::Windows::Forms::ToolStripMenuItem^ objectsToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ collisionToolStripMenuItem;





  private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator4;
  private: System::Windows::Forms::ToolStripButton^  toolStripButton5;
  private: System::Windows::Forms::Label^  label10;

  private: System::Windows::Forms::Panel^  panel2;
  public: static System::Windows::Forms::NumericUpDown^  numericUpDown1;
  private: System::Windows::Forms::Label^  label3;
  public: static System::Windows::Forms::CheckedListBox^  checkedListBox1;
  private: System::Windows::Forms::Label^  label11;
  private: System::Windows::Forms::Label^  label12;
  private: System::Windows::Forms::Panel^  panel5;

  private: System::Windows::Forms::ComboBox^  comboBox3;
  private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem1;
  private: System::Windows::Forms::ToolStripMenuItem^  wireframeToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  solidToolStripMenuItem1;
  private: System::Windows::Forms::ToolStripMenuItem^  texturedToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  transparentToolStripMenuItem1;
  private: System::Windows::Forms::ToolStripMenuItem^  collisionTypeToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  skydomeToolStripMenuItem1;
  private: System::Windows::Forms::ToolStripMenuItem^  railsToolStripMenuItem1;
  private: System::Windows::Forms::ToolStripMenuItem^  respawnsToolStripMenuItem1;
  private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator2;
  private: System::Windows::Forms::ToolStripMenuItem^  showToolsToolStripMenuItem;
  private: System::Windows::Forms::ToolStripButton^  toolStripButton1;
  private: System::Windows::Forms::ToolStripButton^  toolStripButton2;
  private: System::Windows::Forms::ToolStripLabel^  toolStripLabel1;
  private: System::Windows::Forms::Label^  label13;
  private: System::Windows::Forms::Button^ triggerButton1;
  public: System::Windows::Forms::Panel^  panel6;
          public: System::Windows::Forms::TrackBar^ uSlider;
                  public: System::Windows::Forms::TrackBar^ vSlider;
  private: System::ComponentModel::IContainer^  components;


#pragma region Windows Form Designer generated code
           /// <summary>
           /// Required method for Designer support - do not modify
           /// the contents of this method with the code editor.
           /// </summary>
           void InitializeComponent(void)
           {
             this->uSlider = gcnew System::Windows::Forms::TrackBar;
             this->vSlider = gcnew System::Windows::Forms::TrackBar;
             this->properties = gcnew System::Collections::Generic::List<MeshProperties^>();
             System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(THPSLevel::typeid));
             this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
             this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->saveToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->loadToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->importToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->tHPS2ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->tHPS3ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->tHPS4ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->exportToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->tHPS3ToolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->tHUG2ToolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->oBJToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->toolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->wireframeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->solidToolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->texturedToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->transparentToolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->collisionTypeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->skydomeToolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->railsToolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->respawnsToolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->toolStripSeparator2 = (gcnew System::Windows::Forms::ToolStripSeparator());
             this->showToolsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->helpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->aboutToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->toolStrip1 = (gcnew System::Windows::Forms::ToolStrip());
             this->toolStripSplitButton1 = (gcnew System::Windows::Forms::ToolStripSplitButton());
             this->objectModeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->surfaceModeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->railModeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->materialToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
             this->toolStripDropDownButton1 = (gcnew System::Windows::Forms::ToolStripDropDownButton());
             this->objectsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->collisionToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->toolStripButton1 = (gcnew System::Windows::Forms::ToolStripButton());
             this->toolStripButton7 = (gcnew System::Windows::Forms::ToolStripButton());
             this->toolStripButton2 = (gcnew System::Windows::Forms::ToolStripButton());
             this->toolStripButton4 = (gcnew System::Windows::Forms::ToolStripButton());
             this->toolStripButton3 = (gcnew System::Windows::Forms::ToolStripButton());
             this->toolStripButton6 = (gcnew System::Windows::Forms::ToolStripButton());
             this->toolStripSeparator4 = (gcnew System::Windows::Forms::ToolStripSeparator());
             this->toolStripLabel1 = (gcnew System::Windows::Forms::ToolStripLabel());
             this->toolStripButton5 = (gcnew System::Windows::Forms::ToolStripButton());
             this->toolStripButton8 = (gcnew System::Windows::Forms::ToolStripButton());
             this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
             this->toolStripProgressBar1 = (gcnew System::Windows::Forms::ToolStripProgressBar());
             this->toolStripStatusLabel1 = (gcnew System::Windows::Forms::ToolStripStatusLabel());
             this->toolStripStatusLabel2 = (gcnew System::Windows::Forms::ToolStripStatusLabel());
             this->splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
             this->panel6 = (gcnew System::Windows::Forms::Panel());
             this->splitContainer2 = (gcnew System::Windows::Forms::SplitContainer());
             this->listBox1 = (gcnew System::Windows::Forms::ListBox());
             this->label1 = (gcnew System::Windows::Forms::Label());
             this->panel7 = (gcnew System::Windows::Forms::Panel());
             this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
             this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
             this->propertyGrid1 = (gcnew System::Windows::Forms::PropertyGrid());
             this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
             this->listBox2 = (gcnew System::Windows::Forms::ListBox());
             this->panel1 = (gcnew System::Windows::Forms::Panel());
             this->button2 = (gcnew System::Windows::Forms::Button());
             this->button1 = (gcnew System::Windows::Forms::Button());
             this->panel3 = (gcnew System::Windows::Forms::Panel());
             this->label13 = (gcnew System::Windows::Forms::Label());
             this->triggerButton1 = (gcnew System::Windows::Forms::Button());
             this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
             this->label8 = (gcnew System::Windows::Forms::Label());
             this->button4 = (gcnew System::Windows::Forms::Button());
             this->button3 = (gcnew System::Windows::Forms::Button());
             this->label7 = (gcnew System::Windows::Forms::Label());
             this->textBox3 = (gcnew System::Windows::Forms::TextBox());
             this->codeTextBox = (gcnew System::Windows::Forms::TextBox());
             this->label6 = (gcnew System::Windows::Forms::Label());
             this->textBox2 = (gcnew System::Windows::Forms::TextBox());
             this->label5 = (gcnew System::Windows::Forms::Label());
             this->textBox1 = (gcnew System::Windows::Forms::TextBox());
             this->trackBar1 = (gcnew System::Windows::Forms::TrackBar());
             this->label4 = (gcnew System::Windows::Forms::Label());
             this->label2 = (gcnew System::Windows::Forms::Label());
             this->tabPage3 = (gcnew System::Windows::Forms::TabPage());
             this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
             this->panel5 = (gcnew System::Windows::Forms::Panel());
             this->comboBox3 = (gcnew System::Windows::Forms::ComboBox());
             this->label12 = (gcnew System::Windows::Forms::Label());
             this->label11 = (gcnew System::Windows::Forms::Label());
             this->checkedListBox1 = (gcnew System::Windows::Forms::CheckedListBox());
             this->panel2 = (gcnew System::Windows::Forms::Panel());
             this->numericUpDown1 = (gcnew System::Windows::Forms::NumericUpDown());
             this->label3 = (gcnew System::Windows::Forms::Label());
             this->panel4 = (gcnew System::Windows::Forms::Panel());
             this->label10 = (gcnew System::Windows::Forms::Label());
             this->label9 = (gcnew System::Windows::Forms::Label());
             this->comboBox2 = (gcnew System::Windows::Forms::ComboBox());
             this->tabPage4 = (gcnew System::Windows::Forms::TabPage());
             this->draworder = (gcnew System::Windows::Forms::TextBox());
             this->animgroup = (gcnew System::Windows::Forms::GroupBox());
             this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
             this->water = (gcnew System::Windows::Forms::RadioButton());
             this->linear = (gcnew System::Windows::Forms::RadioButton());
             this->label17 = (gcnew System::Windows::Forms::Label());
             this->vvel = (gcnew System::Windows::Forms::TextBox());
             this->label16 = (gcnew System::Windows::Forms::Label());
             this->label15 = (gcnew System::Windows::Forms::Label());
             this->uvel = (gcnew System::Windows::Forms::TextBox());
             this->animated = (gcnew System::Windows::Forms::CheckBox());
             this->BrowseTexture = (gcnew System::Windows::Forms::Button());
             this->MaterialList = (gcnew System::Windows::Forms::TreeView());
             this->doubled = (gcnew System::Windows::Forms::CheckBox());
             this->label14 = (gcnew System::Windows::Forms::Label());
             this->transparent = (gcnew System::Windows::Forms::CheckBox());
             this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
             this->panel9 = (gcnew System::Windows::Forms::Panel());
             this->pictureBox1 = (gcnew System::Windows::Forms::Panel());
             this->menuStrip1->SuspendLayout();
             this->toolStrip1->SuspendLayout();
             this->statusStrip1->SuspendLayout();
             (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->splitContainer1))->BeginInit();
             this->splitContainer1->Panel1->SuspendLayout();
             this->splitContainer1->Panel2->SuspendLayout();
             this->splitContainer1->SuspendLayout();
             (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->splitContainer2))->BeginInit();
             this->splitContainer2->Panel1->SuspendLayout();
             this->splitContainer2->Panel2->SuspendLayout();
             this->splitContainer2->SuspendLayout();
             this->panel7->SuspendLayout();
             this->tabControl1->SuspendLayout();
             this->tabPage1->SuspendLayout();
             this->tabPage2->SuspendLayout();
             this->panel1->SuspendLayout();
             this->panel3->SuspendLayout();
             (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBar1))->BeginInit();
             this->tabPage3->SuspendLayout();
             this->groupBox1->SuspendLayout();
             this->panel5->SuspendLayout();
             this->panel2->SuspendLayout();
             (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown1))->BeginInit();
             this->panel4->SuspendLayout();
             this->tabPage4->SuspendLayout();
             this->animgroup->SuspendLayout();
             this->groupBox3->SuspendLayout();
             this->panel9->SuspendLayout();
             this->SuspendLayout();
             // 
             // menuStrip1
             // 
             this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->fileToolStripMenuItem, 
               this->toolStripMenuItem1, this->helpToolStripMenuItem});
             this->menuStrip1->Location = System::Drawing::Point(0, 0);
             this->menuStrip1->Name = L"menuStrip1";
             this->menuStrip1->Size = System::Drawing::Size(933, 24);
             this->menuStrip1->TabIndex = 1;
             this->menuStrip1->Text = L"menuStrip1";
             // 
             // fileToolStripMenuItem
             // 
             this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {this->saveToolStripMenuItem, 
               this->loadToolStripMenuItem, this->importToolStripMenuItem, this->exportToolStripMenuItem, this->exitToolStripMenuItem});
             this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
             this->fileToolStripMenuItem->Size = System::Drawing::Size(35, 20);
             this->fileToolStripMenuItem->Text = L"&File";
             // 
             // saveToolStripMenuItem
             // 
             this->saveToolStripMenuItem->Name = L"saveToolStripMenuItem";
             this->saveToolStripMenuItem->Size = System::Drawing::Size(110, 22);
             this->saveToolStripMenuItem->Text = L"Save...";
             this->saveToolStripMenuItem->Click += gcnew System::EventHandler(this, &THPSLevel::saveToolStripMenuItem_Click);
             // 
             // loadToolStripMenuItem
             // 
             this->loadToolStripMenuItem->Name = L"loadToolStripMenuItem";
             this->loadToolStripMenuItem->Size = System::Drawing::Size(110, 22);
             this->loadToolStripMenuItem->Text = L"Load...";
             this->loadToolStripMenuItem->Click += gcnew System::EventHandler(this, &THPSLevel::loadToolStripMenuItem_Click);
             // 
             // importToolStripMenuItem
             // 
             this->importToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->tHPS2ToolStripMenuItem, 
               this->tHPS3ToolStripMenuItem, this->tHPS4ToolStripMenuItem});
             this->importToolStripMenuItem->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"importToolStripMenuItem.Image")));
             this->importToolStripMenuItem->Name = L"importToolStripMenuItem";
             this->importToolStripMenuItem->Size = System::Drawing::Size(110, 22);
             this->importToolStripMenuItem->Text = L"Import";
             // 
             // tHPS2ToolStripMenuItem
             // 
             this->tHPS2ToolStripMenuItem->Name = L"tHPS2ToolStripMenuItem";
             this->tHPS2ToolStripMenuItem->Size = System::Drawing::Size(117, 22);
             this->tHPS2ToolStripMenuItem->Text = L"THPS2...";
             this->tHPS2ToolStripMenuItem->Click += gcnew System::EventHandler(this, &THPSLevel::tHPS2ToolStripMenuItem_Click);
             // 
             // tHPS3ToolStripMenuItem
             // 
             this->tHPS3ToolStripMenuItem->Name = L"tHPS3ToolStripMenuItem";
             this->tHPS3ToolStripMenuItem->Size = System::Drawing::Size(117, 22);
             this->tHPS3ToolStripMenuItem->Text = L"THPS3...";
             this->tHPS3ToolStripMenuItem->Click += gcnew System::EventHandler(this, &THPSLevel::tHPS3ToolStripMenuItem_Click);
             // 
             // tHPS4ToolStripMenuItem
             // 
             this->tHPS4ToolStripMenuItem->Name = L"tHPS4ToolStripMenuItem";
             this->tHPS4ToolStripMenuItem->Size = System::Drawing::Size(117, 22);
             this->tHPS4ToolStripMenuItem->Text = L"THPS4+...";
             this->tHPS4ToolStripMenuItem->Click += gcnew System::EventHandler(this, &THPSLevel::tHPS4ToolStripMenuItem_Click);
             // 
             // exportToolStripMenuItem
             // 
             this->exportToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->tHPS3ToolStripMenuItem1, 
               this->tHUG2ToolStripMenuItem1, this->oBJToolStripMenuItem
             });
             this->exportToolStripMenuItem->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"exportToolStripMenuItem.Image")));
             this->exportToolStripMenuItem->Name = L"exportToolStripMenuItem";
             this->exportToolStripMenuItem->Size = System::Drawing::Size(110, 22);
             this->exportToolStripMenuItem->Text = L"Export";
             // 
             // tHPS3ToolStripMenuItem1
             // 
             this->tHPS3ToolStripMenuItem1->Name = L"tHPS3ToolStripMenuItem1";
             this->tHPS3ToolStripMenuItem1->Size = System::Drawing::Size(117, 22);
             this->tHPS3ToolStripMenuItem1->Text = L"THPS3...";
             this->tHPS3ToolStripMenuItem1->Click += gcnew System::EventHandler(this, &THPSLevel::tHPS3ToolStripMenuItem1_Click);
             // 
             // tHUG2ToolStripMenuItem1
             // 
             this->tHUG2ToolStripMenuItem1->Name = L"tHUG2ToolStripMenuItem1";
             this->tHUG2ToolStripMenuItem1->Size = System::Drawing::Size(117, 22);
             this->tHUG2ToolStripMenuItem1->Text = L"THUG2...";
             this->tHUG2ToolStripMenuItem1->Click += gcnew System::EventHandler(this, &THPSLevel::tHUG2ToolStripMenuItem1_Click);
             // 
             // oBJToolStripMenuItem
             // 
             this->oBJToolStripMenuItem->Name = L"oBJToolStripMenuItem";
             this->oBJToolStripMenuItem->Size = System::Drawing::Size(117, 22);
             this->oBJToolStripMenuItem->Text = L"OBJ...";
             // 
             // exitToolStripMenuItem
             // 
             this->exitToolStripMenuItem->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"exitToolStripMenuItem.Image")));
             this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
             this->exitToolStripMenuItem->Size = System::Drawing::Size(110, 22);
             this->exitToolStripMenuItem->Text = L"Exit";
             this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &THPSLevel::exitToolStripMenuItem_Click);
             // 
             // toolStripMenuItem1
             // 
             this->toolStripMenuItem1->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(7) {this->wireframeToolStripMenuItem, 
               this->solidToolStripMenuItem1, this->skydomeToolStripMenuItem1, this->railsToolStripMenuItem1, this->respawnsToolStripMenuItem1, 
               this->toolStripSeparator2, this->showToolsToolStripMenuItem});
             this->toolStripMenuItem1->Name = L"toolStripMenuItem1";
             this->toolStripMenuItem1->Size = System::Drawing::Size(41, 20);
             this->toolStripMenuItem1->Text = L"&View";
             // 
             // wireframeToolStripMenuItem
             // 
             this->wireframeToolStripMenuItem->CheckOnClick = true;
             this->wireframeToolStripMenuItem->Name = L"wireframeToolStripMenuItem";
             this->wireframeToolStripMenuItem->Size = System::Drawing::Size(128, 22);
             this->wireframeToolStripMenuItem->Text = L"Wireframe";
             // 
             // solidToolStripMenuItem1
             // 
             this->solidToolStripMenuItem1->Checked = true;
             this->solidToolStripMenuItem1->CheckOnClick = true;
             this->solidToolStripMenuItem1->CheckState = System::Windows::Forms::CheckState::Checked;
             this->solidToolStripMenuItem1->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->texturedToolStripMenuItem, 
               this->transparentToolStripMenuItem1, this->collisionTypeToolStripMenuItem});
             this->solidToolStripMenuItem1->Name = L"solidToolStripMenuItem1";
             this->solidToolStripMenuItem1->Size = System::Drawing::Size(128, 22);
             this->solidToolStripMenuItem1->Text = L"Solid";
             // 
             // texturedToolStripMenuItem
             // 
             this->texturedToolStripMenuItem->Checked = true;
             this->texturedToolStripMenuItem->CheckOnClick = true;
             this->texturedToolStripMenuItem->CheckState = System::Windows::Forms::CheckState::Checked;
             this->texturedToolStripMenuItem->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"texturedToolStripMenuItem.Image")));
             this->texturedToolStripMenuItem->Name = L"texturedToolStripMenuItem";
             this->texturedToolStripMenuItem->Size = System::Drawing::Size(139, 22);
             this->texturedToolStripMenuItem->Text = L"Textured";
             // 
             // transparentToolStripMenuItem1
             // 
             this->transparentToolStripMenuItem1->Checked = true;
             this->transparentToolStripMenuItem1->CheckOnClick = true;
             this->transparentToolStripMenuItem1->CheckState = System::Windows::Forms::CheckState::Checked;
             this->transparentToolStripMenuItem1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"transparentToolStripMenuItem1.Image")));
             this->transparentToolStripMenuItem1->Name = L"transparentToolStripMenuItem1";
             this->transparentToolStripMenuItem1->Size = System::Drawing::Size(139, 22);
             this->transparentToolStripMenuItem1->Text = L"Transparent";
             // 
             // collisionTypeToolStripMenuItem
             // 
             this->collisionTypeToolStripMenuItem->CheckOnClick = true;
             this->collisionTypeToolStripMenuItem->Enabled = false;
             this->collisionTypeToolStripMenuItem->Name = L"collisionTypeToolStripMenuItem";
             this->collisionTypeToolStripMenuItem->Size = System::Drawing::Size(139, 22);
             this->collisionTypeToolStripMenuItem->Text = L"Collision Type";
             // 
             // skydomeToolStripMenuItem1
             // 
             this->skydomeToolStripMenuItem1->Checked = true;
             this->skydomeToolStripMenuItem1->CheckOnClick = true;
             this->skydomeToolStripMenuItem1->CheckState = System::Windows::Forms::CheckState::Checked;
             this->skydomeToolStripMenuItem1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"skydomeToolStripMenuItem1.Image")));
             this->skydomeToolStripMenuItem1->Name = L"skydomeToolStripMenuItem1";
             this->skydomeToolStripMenuItem1->Size = System::Drawing::Size(128, 22);
             this->skydomeToolStripMenuItem1->Text = L"Skydome";
             // 
             // railsToolStripMenuItem1
             // 
             this->railsToolStripMenuItem1->CheckOnClick = true;
             this->railsToolStripMenuItem1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"railsToolStripMenuItem1.Image")));
             this->railsToolStripMenuItem1->Name = L"railsToolStripMenuItem1";
             this->railsToolStripMenuItem1->Size = System::Drawing::Size(128, 22);
             this->railsToolStripMenuItem1->Text = L"Rails";
             // 
             // respawnsToolStripMenuItem1
             // 
             this->respawnsToolStripMenuItem1->CheckOnClick = true;
             this->respawnsToolStripMenuItem1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"respawnsToolStripMenuItem1.Image")));
             this->respawnsToolStripMenuItem1->Name = L"respawnsToolStripMenuItem1";
             this->respawnsToolStripMenuItem1->Size = System::Drawing::Size(128, 22);
             this->respawnsToolStripMenuItem1->Text = L"Respawns";
             // 
             // toolStripSeparator2
             // 
             this->toolStripSeparator2->Name = L"toolStripSeparator2";
             this->toolStripSeparator2->Size = System::Drawing::Size(125, 6);
             // 
             // showToolsToolStripMenuItem
             // 
             this->showToolsToolStripMenuItem->Checked = true;
             this->showToolsToolStripMenuItem->CheckOnClick = true;
             this->showToolsToolStripMenuItem->CheckState = System::Windows::Forms::CheckState::Checked;
             this->showToolsToolStripMenuItem->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"showToolsToolStripMenuItem.Image")));
             this->showToolsToolStripMenuItem->Name = L"showToolsToolStripMenuItem";
             this->showToolsToolStripMenuItem->Size = System::Drawing::Size(128, 22);
             this->showToolsToolStripMenuItem->Text = L"Show Tools";
             // 
             // helpToolStripMenuItem
             // 
             this->helpToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->aboutToolStripMenuItem});
             this->helpToolStripMenuItem->Name = L"helpToolStripMenuItem";
             this->helpToolStripMenuItem->Size = System::Drawing::Size(40, 20);
             this->helpToolStripMenuItem->Text = L"&Help";
             // 
             // aboutToolStripMenuItem
             // 
             this->aboutToolStripMenuItem->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"aboutToolStripMenuItem.Image")));
             this->aboutToolStripMenuItem->Name = L"aboutToolStripMenuItem";
             this->aboutToolStripMenuItem->Size = System::Drawing::Size(115, 22);
             this->aboutToolStripMenuItem->Text = L"&About...";
             // 
             // toolStrip1
             // 
             this->toolStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(13) {this->toolStripSplitButton1, 
               this->toolStripSeparator1, this->toolStripDropDownButton1, this->toolStripButton1, this->toolStripButton7, this->toolStripButton2, 
               this->toolStripButton4, this->toolStripButton3, this->toolStripButton6, this->toolStripSeparator4, this->toolStripLabel1, this->toolStripButton5, 
               this->toolStripButton8});
             this->toolStrip1->Location = System::Drawing::Point(0, 24);
             this->toolStrip1->Name = L"toolStrip1";
             this->toolStrip1->Size = System::Drawing::Size(933, 25);
             this->toolStrip1->TabIndex = 2;
             this->toolStrip1->Text = L"toolStrip1";
             // 
             // toolStripSplitButton1
             // 
             this->toolStripSplitButton1->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->objectModeToolStripMenuItem, 
               this->surfaceModeToolStripMenuItem, this->railModeToolStripMenuItem, this->materialToolStripMenuItem});
             this->toolStripSplitButton1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripSplitButton1.Image")));
             this->toolStripSplitButton1->ImageTransparentColor = System::Drawing::Color::Magenta;
             this->toolStripSplitButton1->Name = L"toolStripSplitButton1";
             this->toolStripSplitButton1->Size = System::Drawing::Size(100, 22);
             this->toolStripSplitButton1->Text = L"Object Mode";
             this->toolStripSplitButton1->ToolTipText = L"Select the edit mode here.";
             // 
             // objectModeToolStripMenuItem
             // 
             this->objectModeToolStripMenuItem->Name = L"objectModeToolStripMenuItem";
             this->objectModeToolStripMenuItem->Size = System::Drawing::Size(135, 22);
             this->objectModeToolStripMenuItem->Text = L"Object Mode";
             this->objectModeToolStripMenuItem->Click += gcnew System::EventHandler(this, &THPSLevel::objectModeToolStripMenuItem_Click);
             // 
             // surfaceModeToolStripMenuItem
             // 
             this->surfaceModeToolStripMenuItem->Name = L"surfaceModeToolStripMenuItem";
             this->surfaceModeToolStripMenuItem->Size = System::Drawing::Size(135, 22);
             this->surfaceModeToolStripMenuItem->Text = L"Face Mode";
             this->surfaceModeToolStripMenuItem->Click += gcnew System::EventHandler(this, &THPSLevel::surfaceModeToolStripMenuItem_Click);
             // 
             // railModeToolStripMenuItem
             // 
             this->railModeToolStripMenuItem->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"railModeToolStripMenuItem.Image")));
             this->railModeToolStripMenuItem->Name = L"railModeToolStripMenuItem";
             this->railModeToolStripMenuItem->Size = System::Drawing::Size(135, 22);
             this->railModeToolStripMenuItem->Text = L"Node Mode";
             this->railModeToolStripMenuItem->Click += gcnew System::EventHandler(this, &THPSLevel::railModeToolStripMenuItem_Click);
             // 
             // materialToolStripMenuItem
             // 
             this->materialToolStripMenuItem->Name = L"materialToolStripMenuItem";
             this->materialToolStripMenuItem->Size = System::Drawing::Size(135, 22);
             this->materialToolStripMenuItem->Text = L"Material";
             this->materialToolStripMenuItem->Click += gcnew System::EventHandler(this, &THPSLevel::materialToolStripMenuItem_Click);
             // 
             // toolStripSeparator1
             // 
             this->toolStripSeparator1->Name = L"toolStripSeparator1";
             this->toolStripSeparator1->Size = System::Drawing::Size(6, 25);
             // 
             // toolStripDropDownButton1
             // 
             this->toolStripDropDownButton1->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->objectsToolStripMenuItem, 
               this->collisionToolStripMenuItem});
             this->toolStripDropDownButton1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripDropDownButton1.Image")));
             this->toolStripDropDownButton1->ImageTransparentColor = System::Drawing::Color::Magenta;
             this->toolStripDropDownButton1->Name = L"toolStripDropDownButton1";
             this->toolStripDropDownButton1->Size = System::Drawing::Size(97, 22);
             this->toolStripDropDownButton1->Text = L"View Meshes";
             // 
             // objectsToolStripMenuItem
             // 
             this->objectsToolStripMenuItem->Name = L"objectsToolStripMenuItem";
             this->objectsToolStripMenuItem->Size = System::Drawing::Size(112, 22);
             this->objectsToolStripMenuItem->Text = L"Meshes";
             this->objectsToolStripMenuItem->Click += gcnew System::EventHandler(this, &THPSLevel::objectsToolStripMenuItem_Click);
             // 
             // collisionToolStripMenuItem
             // 
             this->collisionToolStripMenuItem->Name = L"collisionToolStripMenuItem";
             this->collisionToolStripMenuItem->Size = System::Drawing::Size(112, 22);
             this->collisionToolStripMenuItem->Text = L"Collision";
             this->collisionToolStripMenuItem->Click += gcnew System::EventHandler(this, &THPSLevel::collisionToolStripMenuItem_Click);
             // 
             // toolStripButton1
             // 
             this->toolStripButton1->Checked = true;
             this->toolStripButton1->CheckOnClick = true;
             this->toolStripButton1->CheckState = System::Windows::Forms::CheckState::Checked;
             this->toolStripButton1->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
             this->toolStripButton1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripButton1.Image")));
             this->toolStripButton1->ImageTransparentColor = System::Drawing::Color::Magenta;
             this->toolStripButton1->Name = L"toolStripButton1";
             this->toolStripButton1->Size = System::Drawing::Size(23, 22);
             this->toolStripButton1->Text = L"Show Textures";
             this->toolStripButton1->ToolTipText = L"Show Textures";
             this->toolStripButton1->Click += gcnew System::EventHandler(this, &THPSLevel::toolStripButton1_Click);
             // 
             // toolStripButton7
             // 
             this->toolStripButton7->Checked = true;
             this->toolStripButton7->CheckOnClick = true;
             this->toolStripButton7->CheckState = System::Windows::Forms::CheckState::Checked;
             this->toolStripButton7->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
             this->toolStripButton7->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripButton7.Image")));
             this->toolStripButton7->ImageTransparentColor = System::Drawing::Color::Magenta;
             this->toolStripButton7->Name = L"toolStripButton7";
             this->toolStripButton7->Size = System::Drawing::Size(23, 22);
             this->toolStripButton7->Text = L"Show Sky";
             this->toolStripButton7->Click += gcnew System::EventHandler(this, &THPSLevel::toolStripButton7_Click);
             // 
             // toolStripButton2
             // 
             this->toolStripButton2->Checked = true;
             this->toolStripButton2->CheckOnClick = true;
             this->toolStripButton2->CheckState = System::Windows::Forms::CheckState::Checked;
             this->toolStripButton2->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
             this->toolStripButton2->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripButton2.Image")));
             this->toolStripButton2->ImageTransparentColor = System::Drawing::Color::Magenta;
             this->toolStripButton2->Name = L"toolStripButton2";
             this->toolStripButton2->Size = System::Drawing::Size(23, 22);
             this->toolStripButton2->Text = L"Enable Transparency";
             this->toolStripButton2->ToolTipText = L"Enable Transparency";
             this->toolStripButton2->Click += gcnew System::EventHandler(this, &THPSLevel::toolStripButton2_Click);
             // 
             // toolStripButton4
             // 
             this->toolStripButton4->Checked = true;
             this->toolStripButton4->CheckOnClick = true;
             this->toolStripButton4->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
             this->toolStripButton4->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripButton4.Image")));
             this->toolStripButton4->ImageTransparentColor = System::Drawing::Color::Magenta;
             this->toolStripButton4->Name = L"toolStripButton4";
             this->toolStripButton4->Size = System::Drawing::Size(23, 22);
             this->toolStripButton4->Text = L"Show Rails";
             this->toolStripButton4->ToolTipText = L"Show Rails";
             this->toolStripButton4->Click += gcnew System::EventHandler(this, &THPSLevel::toolStripButton4_Click);
             // 
             // toolStripButton3
             // 
             this->toolStripButton3->Checked = true;
             this->toolStripButton3->CheckOnClick = true;
             this->toolStripButton3->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
             this->toolStripButton3->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripButton3.Image")));
             this->toolStripButton3->ImageTransparentColor = System::Drawing::Color::Magenta;
             this->toolStripButton3->Name = L"toolStripButton3";
             this->toolStripButton3->Size = System::Drawing::Size(23, 22);
             this->toolStripButton3->Text = L"Show Respawns";
             this->toolStripButton3->ToolTipText = L"Show Respawns";
             this->toolStripButton3->Click += gcnew System::EventHandler(this, &THPSLevel::toolStripButton3_Click);
             // 
             // toolStripButton6
             // 
             this->toolStripButton6->CheckOnClick = true;
             this->toolStripButton6->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
             this->toolStripButton6->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripButton6.Image")));
             this->toolStripButton6->ImageTransparentColor = System::Drawing::Color::Magenta;
             this->toolStripButton6->Name = L"toolStripButton6";
             this->toolStripButton6->Size = System::Drawing::Size(23, 22);
             this->toolStripButton6->Text = L"Show CTF Flags";
             // 
             // toolStripSeparator4
             // 
             this->toolStripSeparator4->Name = L"toolStripSeparator4";
             this->toolStripSeparator4->Size = System::Drawing::Size(6, 25);
             // 
             // toolStripLabel1
             // 
             this->toolStripLabel1->Name = L"toolStripLabel1";
             this->toolStripLabel1->Size = System::Drawing::Size(36, 22);
             this->toolStripLabel1->Text = L"Tools:";
             // 
             // toolStripButton5
             // 
             this->toolStripButton5->Checked = true;
             this->toolStripButton5->CheckOnClick = true;
             this->toolStripButton5->CheckState = System::Windows::Forms::CheckState::Checked;
             this->toolStripButton5->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
             this->toolStripButton5->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripButton5.Image")));
             this->toolStripButton5->ImageTransparentColor = System::Drawing::Color::Magenta;
             this->toolStripButton5->Name = L"toolStripButton5";
             this->toolStripButton5->Size = System::Drawing::Size(23, 22);
             this->toolStripButton5->Text = L"Show Tools";
             this->toolStripButton5->Click += gcnew System::EventHandler(this, &THPSLevel::toolStripButton5_Click);
             // 
             // toolStripButton8
             // 
             this->toolStripButton8->Checked = false;
             this->toolStripButton8->CheckState = System::Windows::Forms::CheckState::Unchecked;
             this->toolStripButton8->CheckOnClick = true;
             this->toolStripButton8->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
             this->toolStripButton8->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripButton8.Image")));
             this->toolStripButton8->ImageTransparentColor = System::Drawing::Color::Magenta;
             this->toolStripButton8->Name = L"toolStripButton8";
             this->toolStripButton8->Size = System::Drawing::Size(23, 22);
             this->toolStripButton8->Text = L"Quick Material Mode";
             this->toolStripButton8->Click += gcnew System::EventHandler(this, &THPSLevel::toolStripButton8_Click);
             // 
             // statusStrip1
             // 
             this->statusStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->toolStripProgressBar1, 
               this->toolStripStatusLabel1, this->toolStripStatusLabel2});
             this->statusStrip1->Location = System::Drawing::Point(0, 554);
             this->statusStrip1->Name = L"statusStrip1";
             this->statusStrip1->Size = System::Drawing::Size(933, 22);
             this->statusStrip1->TabIndex = 3;
             this->statusStrip1->Text = L"statusStrip1";
             // 
             // toolStripProgressBar1
             // 
             this->toolStripProgressBar1->Name = L"toolStripProgressBar1";
             this->toolStripProgressBar1->Size = System::Drawing::Size(100, 16);
             // 
             // toolStripStatusLabel1
             // 
             this->toolStripStatusLabel1->Name = L"toolStripStatusLabel1";
             this->toolStripStatusLabel1->Size = System::Drawing::Size(12, 17);
             this->toolStripStatusLabel1->Text = L"\?";
             // 
             // toolStripStatusLabel2
             // 
             this->toolStripStatusLabel2->Name = L"toolStripStatusLabel2";
             this->toolStripStatusLabel2->Size = System::Drawing::Size(804, 17);
             this->toolStripStatusLabel2->Spring = true;
             this->toolStripStatusLabel2->Text = L"THPS Level Editor v1.0.1";
             this->toolStripStatusLabel2->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
             // 
             // splitContainer1
             // 
             this->splitContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
             this->splitContainer1->Location = System::Drawing::Point(0, 49);
             this->splitContainer1->Name = L"splitContainer1";
             // 
             // splitContainer1.Panel1
             // 
             this->splitContainer1->Panel1->Controls->Add(this->panel6);
             this->splitContainer1->Panel1->Padding = System::Windows::Forms::Padding(6, 6, 3, 6);
             // 
             // splitContainer1.Panel2
             // 
             this->splitContainer1->Panel2->Controls->Add(this->splitContainer2);
             this->splitContainer1->Size = System::Drawing::Size(933, 505);
             this->splitContainer1->SplitterDistance = 618;
             this->splitContainer1->TabIndex = 4;
             // 
             // panel6
             // 
             this->panel6->AutoSize = true;
             this->panel6->BackColor = System::Drawing::SystemColors::AppWorkspace;
             this->panel6->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
             this->panel6->Dock = System::Windows::Forms::DockStyle::Fill;
             this->panel6->ForeColor = System::Drawing::SystemColors::ControlText;
             this->panel6->Location = System::Drawing::Point(6, 6);
             this->panel6->Name = L"panel6";
             this->panel6->Size = System::Drawing::Size(609, 493);
             this->panel6->TabIndex = 0;
             this->panel6->GotFocus += gcnew System::EventHandler(this, &THPSLevel::RenderGotFocus);
             this->panel6->LostFocus += gcnew System::EventHandler(this, &THPSLevel::RenderLostFocus);
             this->panel6->Resize += gcnew System::EventHandler(this, &THPSLevel::ResizeRenderWindow);
             // 
             // splitContainer2
             // 
             this->splitContainer2->Dock = System::Windows::Forms::DockStyle::Fill;
             this->splitContainer2->Location = System::Drawing::Point(0, 0);
             this->splitContainer2->Name = L"splitContainer2";
             this->splitContainer2->Orientation = System::Windows::Forms::Orientation::Horizontal;
             // 
             // splitContainer2.Panel1
             // 
             this->splitContainer2->Panel1->Controls->Add(this->listBox1);
             this->splitContainer2->Panel1->Controls->Add(this->label1);
             this->splitContainer2->Panel1->Padding = System::Windows::Forms::Padding(3, 6, 8, 3);
             // 
             // splitContainer2.Panel2
             // 
             this->splitContainer2->Panel2->Controls->Add(this->panel7);
             this->splitContainer2->Size = System::Drawing::Size(311, 505);
             this->splitContainer2->SplitterDistance = 175;
             this->splitContainer2->TabIndex = 0;
             // 
             // listBox1
             // 
             this->listBox1->Dock = System::Windows::Forms::DockStyle::Fill;
             this->listBox1->FormattingEnabled = true;
             this->listBox1->Location = System::Drawing::Point(3, 19);
             this->listBox1->Margin = System::Windows::Forms::Padding(0);
             this->listBox1->Name = L"listBox1";
             this->listBox1->SelectionMode = System::Windows::Forms::SelectionMode::MultiExtended;
             this->listBox1->Size = System::Drawing::Size(300, 153);
             this->listBox1->TabIndex = 1;
             this->listBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &THPSLevel::listBox1_SelectedIndexChanged);
             // 
             // label1
             // 
             this->label1->AutoSize = true;
             this->label1->Dock = System::Windows::Forms::DockStyle::Top;
             this->label1->Location = System::Drawing::Point(3, 6);
             this->label1->Margin = System::Windows::Forms::Padding(3, 0, 3, 3);
             this->label1->Name = L"label1";
             this->label1->Size = System::Drawing::Size(60, 13);
             this->label1->TabIndex = 0;
             this->label1->Text = L"All Objects:";
             // 
             // panel7
             // 
             this->panel7->Controls->Add(this->tabControl1);
             this->panel7->Dock = System::Windows::Forms::DockStyle::Fill;
             this->panel7->Location = System::Drawing::Point(0, 0);
             this->panel7->Name = L"panel7";
             this->panel7->Padding = System::Windows::Forms::Padding(3, 3, 6, 6);
             this->panel7->Size = System::Drawing::Size(311, 326);
             this->panel7->TabIndex = 1;
             // 
             // tabControl1
             // 
             this->tabControl1->Controls->Add(this->tabPage1);
             this->tabControl1->Controls->Add(this->tabPage2);
             this->tabControl1->Controls->Add(this->tabPage3);
             this->tabControl1->Controls->Add(this->tabPage4);
             this->tabControl1->Dock = System::Windows::Forms::DockStyle::Fill;
             this->tabControl1->Location = System::Drawing::Point(3, 3);
             this->tabControl1->Name = L"tabControl1";
             this->tabControl1->SelectedIndex = 0;
             this->tabControl1->Size = System::Drawing::Size(302, 317);
             this->tabControl1->TabIndex = 0;
             // 
             // tabPage1
             // 
             this->tabPage1->Controls->Add(this->propertyGrid1);
             this->tabPage1->ForeColor = System::Drawing::SystemColors::ControlText;
             this->tabPage1->Location = System::Drawing::Point(4, 22);
             this->tabPage1->Name = L"tabPage1";
             this->tabPage1->Padding = System::Windows::Forms::Padding(6);
             this->tabPage1->Size = System::Drawing::Size(294, 291);
             this->tabPage1->TabIndex = 0;
             this->tabPage1->Text = L"Properties";
             this->tabPage1->UseVisualStyleBackColor = true;
             // 
             // propertyGrid1
             // 
             this->propertyGrid1->BackColor = System::Drawing::SystemColors::Window;
             this->propertyGrid1->Dock = System::Windows::Forms::DockStyle::Fill;
             this->propertyGrid1->Location = System::Drawing::Point(6, 6);
             this->propertyGrid1->Name = L"propertyGrid1";
             this->propertyGrid1->Size = System::Drawing::Size(282, 279);
             this->propertyGrid1->TabIndex = 1;
             // 
             // tabPage2
             // 
             this->tabPage2->Controls->Add(this->listBox2);
             this->tabPage2->Controls->Add(this->panel1);
             this->tabPage2->Controls->Add(this->panel3);
             this->tabPage2->Controls->Add(this->label2);
             this->tabPage2->ForeColor = System::Drawing::SystemColors::ControlText;
             this->tabPage2->Location = System::Drawing::Point(4, 22);
             this->tabPage2->Name = L"tabPage2";
             this->tabPage2->Padding = System::Windows::Forms::Padding(8);
             this->tabPage2->Size = System::Drawing::Size(294, 291);
             this->tabPage2->TabIndex = 1;
             this->tabPage2->Text = L"Rails";
             this->tabPage2->UseVisualStyleBackColor = true;
             // 
             // listBox2
             // 
             this->listBox2->Dock = System::Windows::Forms::DockStyle::Fill;
             this->listBox2->FormattingEnabled = true;
             this->listBox2->Location = System::Drawing::Point(8, 21);
             this->listBox2->Name = L"listBox2";
             this->listBox2->Size = System::Drawing::Size(198, 106);
             this->listBox2->TabIndex = 3;
             // 
             // panel1
             // 
             this->panel1->Controls->Add(this->button2);
             this->panel1->Controls->Add(this->button1);
             this->panel1->Dock = System::Windows::Forms::DockStyle::Right;
             this->panel1->Location = System::Drawing::Point(206, 21);
             this->panel1->Name = L"panel1";
             this->panel1->Padding = System::Windows::Forms::Padding(5, 0, 0, 0);
             this->panel1->Size = System::Drawing::Size(80, 106);
             this->panel1->TabIndex = 2;
             // 
             // button2
             // 
             this->button2->Dock = System::Windows::Forms::DockStyle::Top;
             this->button2->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"button2.Image")));
             this->button2->Location = System::Drawing::Point(5, 23);
             this->button2->Name = L"button2";
             this->button2->Size = System::Drawing::Size(75, 23);
             this->button2->TabIndex = 1;
             this->button2->Text = L"Remove";
             this->button2->TextImageRelation = System::Windows::Forms::TextImageRelation::ImageBeforeText;
             this->button2->UseVisualStyleBackColor = true;
             // 
             // button1
             // 
             this->button1->Dock = System::Windows::Forms::DockStyle::Top;
             this->button1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"button1.Image")));
             this->button1->Location = System::Drawing::Point(5, 0);
             this->button1->Name = L"button1";
             this->button1->Size = System::Drawing::Size(75, 23);
             this->button1->TabIndex = 0;
             this->button1->Text = L"Add";
             this->button1->TextImageRelation = System::Windows::Forms::TextImageRelation::ImageBeforeText;
             this->button1->UseVisualStyleBackColor = true;
             this->button1->Click += gcnew System::EventHandler(this, &THPSLevel::button1_Click);
             // 
             // panel3
             // 
             this->panel3->Controls->Add(this->label13);
             this->panel3->Controls->Add(this->comboBox1);
             this->panel3->Controls->Add(this->label8);
             this->panel3->Controls->Add(this->button4);
             this->panel3->Controls->Add(this->button3);
             this->panel3->Controls->Add(this->label7);
             this->panel3->Controls->Add(this->textBox3);
             this->panel3->Controls->Add(this->label6);
             this->panel3->Controls->Add(this->textBox2);
             this->panel3->Controls->Add(this->label5);
             this->panel3->Controls->Add(this->textBox1);
             this->panel3->Controls->Add(this->trackBar1);
             this->panel3->Controls->Add(this->label4);
             this->panel3->Dock = System::Windows::Forms::DockStyle::Bottom;
             this->panel3->Location = System::Drawing::Point(8, 127);
             this->panel3->Name = L"panel3";
             this->panel3->Size = System::Drawing::Size(278, 156);
             this->panel3->TabIndex = 0;
             // 
             // label13
             // 
             this->label13->AutoSize = true;
             this->label13->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"label13.Image")));
             this->label13->Location = System::Drawing::Point(10, 125);
             this->label13->MinimumSize = System::Drawing::Size(16, 16);
             this->label13->Name = L"label13";
             this->label13->Size = System::Drawing::Size(16, 16);
             this->label13->TabIndex = 0; 
             //this->label13->Visible = false;

             /*//this->triggerButton1->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
             this->triggerButton1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"label13.Image")));
             //this->triggerButton1->ImageTransparentColor = System::Drawing::Color::Magenta;
             this->triggerButton1->Name = L"triggerButton1";
             this->triggerButton1->Location = System::Drawing::Point(10, 125);
             this->triggerButton1->Size = System::Drawing::Size(16, 16);
             this->triggerButton1->Text = L"Edit Script";
             this->triggerButton1->Click += gcnew System::EventHandler(this, &THPSLevel::triggerButton1_Click);*/


             // 
             // comboBox1
             // 
             this->comboBox1->FormattingEnabled = true;
             this->comboBox1->Location = System::Drawing::Point(81, 122);
             this->comboBox1->Name = L"comboBox1";
             this->comboBox1->Size = System::Drawing::Size(174, 21);
             this->comboBox1->TabIndex = 12;
             this->comboBox1->Text = L"NorthRailGap";
             // 
             // label8
             // 
             this->label8->AutoSize = true;
             this->label8->Location = System::Drawing::Point(32, 125);
             this->label8->Name = L"label8";
             this->label8->Size = System::Drawing::Size(43, 13);
             this->label8->TabIndex = 11;
             this->label8->Text = L"Trigger:";
             // 
             // button4
             // 
             this->button4->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"button4.Image")));
             this->button4->Location = System::Drawing::Point(151, 73);
             this->button4->Name = L"button4";
             this->button4->Size = System::Drawing::Size(104, 23);
             this->button4->TabIndex = 9;
             this->button4->Text = L"Remove Node";
             this->button4->TextImageRelation = System::Windows::Forms::TextImageRelation::ImageBeforeText;
             this->button4->UseVisualStyleBackColor = true;
             // 
             // button3
             // 
             this->button3->Location = System::Drawing::Point(151, 44);
             this->button3->Name = L"button3";
             this->button3->Size = System::Drawing::Size(104, 23);
             this->button3->TabIndex = 8;
             this->button3->Text = L"Copy Selection";
             this->button3->UseVisualStyleBackColor = true;
             // 
             // label7
             // 
             this->label7->AutoSize = true;
             this->label7->Location = System::Drawing::Point(10, 99);
             this->label7->Name = L"label7";
             this->label7->Size = System::Drawing::Size(14, 13);
             this->label7->TabIndex = 7;
             this->label7->Text = L"Z";
             // 
             // textBox3
             // 
             this->textBox3->Location = System::Drawing::Point(30, 96);
             this->textBox3->Name = L"textBox3";
             this->textBox3->Size = System::Drawing::Size(100, 20);
             this->textBox3->TabIndex = 6;
             // 
             // label6
             // 
             this->label6->AutoSize = true;
             this->label6->Location = System::Drawing::Point(10, 73);
             this->label6->Name = L"label6";
             this->label6->Size = System::Drawing::Size(14, 13);
             this->label6->TabIndex = 5;
             this->label6->Text = L"Y";
             // 
             // textBox2
             // 
             this->textBox2->Location = System::Drawing::Point(30, 70);
             this->textBox2->Name = L"textBox2";
             this->textBox2->Size = System::Drawing::Size(100, 20);
             this->textBox2->TabIndex = 4;
             // 
             // label5
             // 
             this->label5->AutoSize = true;
             this->label5->Location = System::Drawing::Point(10, 47);
             this->label5->Name = L"label5";
             this->label5->Size = System::Drawing::Size(14, 13);
             this->label5->TabIndex = 3;
             this->label5->Text = L"X";
             // 
             // textBox1
             // 
             this->textBox1->Location = System::Drawing::Point(30, 44);
             this->textBox1->Name = L"textBox1";
             this->textBox1->Size = System::Drawing::Size(100, 20);
             this->textBox1->TabIndex = 2;
             // 
             // trackBar1
             // 
             this->trackBar1->BackColor = System::Drawing::SystemColors::Control;
             this->trackBar1->Location = System::Drawing::Point(3, 19);
             this->trackBar1->Name = L"trackBar1";
             this->trackBar1->Size = System::Drawing::Size(254, 42);
             this->trackBar1->TabIndex = 1;
             this->trackBar1->TickStyle = System::Windows::Forms::TickStyle::None;
             this->trackBar1->Value = 4;
             // 
             // label4
             // 
             this->label4->AutoSize = true;
             this->label4->Location = System::Drawing::Point(3, 3);
             this->label4->Name = L"label4";
             this->label4->Size = System::Drawing::Size(85, 13);
             this->label4->TabIndex = 0;
             this->label4->Text = L"Railnode 4 of 12";
             // 
             // label2
             // 
             this->label2->AutoSize = true;
             this->label2->Dock = System::Windows::Forms::DockStyle::Top;
             this->label2->Location = System::Drawing::Point(8, 8);
             this->label2->Name = L"label2";
             this->label2->Size = System::Drawing::Size(44, 13);
             this->label2->TabIndex = 1;
             this->label2->Text = L"Rail List";
             // 
             // tabPage3
             // 
             this->tabPage3->Controls->Add(this->groupBox1);
             this->tabPage3->Controls->Add(this->panel4);
             this->tabPage3->ForeColor = System::Drawing::SystemColors::ControlText;
             this->tabPage3->Location = System::Drawing::Point(4, 22);
             this->tabPage3->Name = L"tabPage3";
             this->tabPage3->Padding = System::Windows::Forms::Padding(8);
             this->tabPage3->Size = System::Drawing::Size(294, 291);
             this->tabPage3->TabIndex = 2;
             this->tabPage3->Text = L"Collision";
             this->tabPage3->UseVisualStyleBackColor = true;
             // 
             // groupBox1
             // 
             this->groupBox1->Controls->Add(this->panel5);
             this->groupBox1->Controls->Add(this->checkedListBox1);
             this->groupBox1->Controls->Add(this->panel2);
             this->groupBox1->Dock = System::Windows::Forms::DockStyle::Top;
             this->groupBox1->Location = System::Drawing::Point(8, 34);
             this->groupBox1->Name = L"groupBox1";
             this->groupBox1->Padding = System::Windows::Forms::Padding(8);
             this->groupBox1->Size = System::Drawing::Size(278, 176);
             this->groupBox1->TabIndex = 16;
             this->groupBox1->TabStop = false;
             this->groupBox1->Text = L"Faces";
             // 
             // panel5
             // 
             this->panel5->Controls->Add(this->comboBox3);
             this->panel5->Controls->Add(this->label12);
             this->panel5->Controls->Add(this->label11);
             this->panel5->Location = System::Drawing::Point(0, 147);
             this->panel5->Name = L"panel5";
             this->panel5->Size = System::Drawing::Size(271, 22);
             this->panel5->TabIndex = 2;
             // 
             // comboBox3
             // 
             this->comboBox3->Dock = System::Windows::Forms::DockStyle::Fill;
             this->comboBox3->FormattingEnabled = true;
             this->comboBox3->Location = System::Drawing::Point(88, 0);
             this->comboBox3->Name = L"comboBox3";
             this->comboBox3->Size = System::Drawing::Size(183, 21);
             this->comboBox3->TabIndex = 4;
             this->comboBox3->Text = L"HitTheRailSound";
             // 
             // label12
             // 
             this->label12->AutoSize = true;
             this->label12->Dock = System::Windows::Forms::DockStyle::Left;
             this->label12->Location = System::Drawing::Point(16, 0);
             this->label12->Name = L"label12";
             this->label12->Size = System::Drawing::Size(72, 13);
             this->label12->TabIndex = 3;
             this->label12->Text = L"Sound Effect:";
             // 
             // label11
             // 
             this->label11->AutoSize = true;
             this->label11->Dock = System::Windows::Forms::DockStyle::Left;
             this->label11->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"label11.Image")));
             this->label11->Location = System::Drawing::Point(0, 0);
             this->label11->MinimumSize = System::Drawing::Size(16, 16);
             this->label11->Name = L"label11";
             this->label11->Size = System::Drawing::Size(16, 16);
             this->label11->TabIndex = 2;
             // 
             // checkedListBox1
             // 
             this->checkedListBox1->FormattingEnabled = true;
             this->checkedListBox1->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"Skateable", L"Wallrideable", L"Is Ramp", 
               L"Collides", L"Show Shadow", L"Is Trigger"});
             this->checkedListBox1->Location = System::Drawing::Point(2, 47);
             this->checkedListBox1->Name = L"checkedListBox1";
             this->checkedListBox1->Size = System::Drawing::Size(269, 94);
             this->checkedListBox1->TabIndex = 1;
             this->checkedListBox1->ItemCheck += gcnew System::Windows::Forms::ItemCheckEventHandler(this, &THPSLevel::checkedListBox1_itemCheck);
             // 
             // panel2
             // 
             this->panel2->Controls->Add(this->numericUpDown1);
             this->panel2->Controls->Add(this->label3);
             this->panel2->Location = System::Drawing::Point(0, 19);
             this->panel2->Name = L"panel2";
             this->panel2->Size = System::Drawing::Size(271, 22);
             this->panel2->TabIndex = 0;
             // 
             // numericUpDown1
             // 
             this->numericUpDown1->Location = System::Drawing::Point(79, 2);
             this->numericUpDown1->Name = L"numericUpDown1";
             this->numericUpDown1->Size = System::Drawing::Size(54, 20);
             this->numericUpDown1->TabIndex = 3;
             // 
             // label3
             // 
             this->label3->AutoSize = true;
             this->label3->Location = System::Drawing::Point(-1, 4);
             this->label3->Name = L"label3";
             this->label3->Size = System::Drawing::Size(78, 13);
             this->label3->TabIndex = 2;
             this->label3->Text = L"Collision Face: ";
             // 
             // panel4
             // 
             this->panel4->Controls->Add(this->label10);
             this->panel4->Controls->Add(this->codeTextBox);
             this->panel4->Controls->Add(this->triggerButton1);
             this->panel4->Controls->Add(this->label9);
             this->panel4->Controls->Add(this->comboBox2);
             this->panel4->Dock = System::Windows::Forms::DockStyle::Top;
             this->panel4->Location = System::Drawing::Point(8, 8);
             this->panel4->Name = L"panel4";
             this->panel4->Size = System::Drawing::Size(278, 26);
             this->panel4->TabIndex = 15;
             // 
             // label10
             // 
             this->label10->AutoSize = true;
             this->label10->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"label10.Image")));
             this->label10->Location = System::Drawing::Point(11, 3);
             this->label10->MinimumSize = System::Drawing::Size(16, 16);
             this->label10->Name = L"label10";
             this->label10->Size = System::Drawing::Size(16, 16);
             this->label10->TabIndex = 15;
             this->label10->Visible = false;

             this->codeTextBox->Location = System::Drawing::Point(27, 0);
             this->codeTextBox->Name = L"codeTextBox";
             this->codeTextBox->Size = System::Drawing::Size(100, 20);
             this->codeTextBox->Visible = false;

             //this->triggerButton1->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
             this->triggerButton1->AutoSize = true;
             this->triggerButton1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"label10.Image")));
             //this->triggerButton1->ImageTransparentColor = System::Drawing::Color::Magenta;
             this->triggerButton1->Name = L"triggerButton1";
             this->triggerButton1->Location = System::Drawing::Point(11, 3);
             this->triggerButton1->MinimumSize = System::Drawing::Size(16, 16);
             this->triggerButton1->Size = System::Drawing::Size(16, 16);
             this->triggerButton1->Text = L"";//L"Edit Script";
             this->triggerButton1->MouseClick += gcnew System::Windows::Forms::MouseEventHandler(this, &THPSLevel::triggerButton1_Click);
			 this->triggerButton1->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &THPSLevel::FKeyDown);
			 this->triggerButton1->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &THPSLevel::FKeyPress);

             // 
             // label9
             // 
             this->label9->AutoSize = true;
             this->label9->ImageAlign = System::Drawing::ContentAlignment::MiddleLeft;
             this->label9->Location = System::Drawing::Point(33, 6);
             this->label9->Name = L"label9";
             this->label9->Size = System::Drawing::Size(43, 13);
             this->label9->TabIndex = 13;
             this->label9->Text = L"Trigger:";
             this->label9->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
             // 
             // comboBox2
             // 
             this->comboBox2->FormattingEnabled = true;
             this->comboBox2->Location = System::Drawing::Point(82, 3);
             this->comboBox2->Name = L"comboBox2";
             this->comboBox2->Size = System::Drawing::Size(143, 21);
             this->comboBox2->TabIndex = 14;
             this->comboBox2->Text = L"NorthRailGap";
             // 
             // tabPage4
             // 
             this->tabPage4->Controls->Add(this->panel9);
             this->tabPage4->Location = System::Drawing::Point(4, 22);
             this->tabPage4->Name = L"tabPage4";
             this->tabPage4->Padding = System::Windows::Forms::Padding(3);
             this->tabPage4->Size = System::Drawing::Size(294, 291);
             this->tabPage4->TabIndex = 3;
             this->tabPage4->Text = L"Material";
             this->tabPage4->UseVisualStyleBackColor = true;
             // 
             // draworder
             // 
             this->draworder->Location = System::Drawing::Point(3, 159);
             this->draworder->Name = L"draworder";
             this->draworder->Size = System::Drawing::Size(89, 20);
             this->draworder->TabIndex = 11;
             this->draworder->TextChanged += gcnew System::EventHandler(this, &THPSLevel::draworder_TextChanged);
             this->draworder->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &THPSLevel::AllowNumbersOnly);
             // 
             // animgroup
             // 
             this->animgroup->Controls->Add(this->groupBox3);
             this->animgroup->Controls->Add(this->label17);
             this->animgroup->Controls->Add(this->vvel);
             this->animgroup->Controls->Add(this->label16);
             this->animgroup->Controls->Add(this->label15);
             this->animgroup->Controls->Add(this->uvel);
             this->animgroup->Enabled = false;
             this->animgroup->Location = System::Drawing::Point(3, 208);
             this->animgroup->Name = L"animgroup";
             this->animgroup->Size = System::Drawing::Size(282, 77);
             this->animgroup->TabIndex = 10;
             this->animgroup->TabStop = false;
             this->animgroup->Text = L"Animation";
             // 
             // groupBox3
             // 
             this->groupBox3->Controls->Add(this->water);
             this->groupBox3->Controls->Add(this->linear);
             this->groupBox3->Location = System::Drawing::Point(133, 16);
             this->groupBox3->Name = L"groupBox3";
             this->groupBox3->Size = System::Drawing::Size(143, 55);
             this->groupBox3->TabIndex = 16;
             this->groupBox3->TabStop = false;
             this->groupBox3->Text = L"Type";
             // 
             // water
             // 
             this->water->AutoSize = true;
             this->water->Location = System::Drawing::Point(6, 27);
             this->water->Name = L"water";
             this->water->Size = System::Drawing::Size(54, 17);
             this->water->TabIndex = 15;
             this->water->TabStop = true;
             this->water->Text = L"Water";
             this->water->UseVisualStyleBackColor = true;
             this->water->CheckedChanged += gcnew System::EventHandler(this, &THPSLevel::water_CheckedChanged);
             // 
             // linear
             // 
             this->linear->AutoSize = true;
             this->linear->Location = System::Drawing::Point(69, 27);
             this->linear->Name = L"linear";
             this->linear->Size = System::Drawing::Size(54, 17);
             this->linear->TabIndex = 14;
             this->linear->TabStop = true;
             this->linear->Text = L"Linear";
             this->linear->UseVisualStyleBackColor = true;
             this->linear->CheckedChanged += gcnew System::EventHandler(this, &THPSLevel::linear_CheckedChanged);
             // 
             // label17
             // 
             this->label17->AutoSize = true;
             this->label17->Location = System::Drawing::Point(127, 20);
             this->label17->Name = L"label17";
             this->label17->Size = System::Drawing::Size(0, 13);
             this->label17->TabIndex = 13;
             // 
             // vvel
             // 
             this->vvel->Location = System::Drawing::Point(47, 44);
             this->vvel->Name = L"vvel";
             this->vvel->Size = System::Drawing::Size(73, 20);
             this->vvel->TabIndex = 12;
             this->vvel->TextChanged += gcnew System::EventHandler(this, &THPSLevel::vvel_TextChanged);
             this->vvel->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &THPSLevel::AllowNumbersOnly);
             // 
             // label16
             // 
             this->label16->AutoSize = true;
             this->label16->Location = System::Drawing::Point(8, 47);
             this->label16->Name = L"label16";
             this->label16->Size = System::Drawing::Size(32, 13);
             this->label16->TabIndex = 11;
             this->label16->Text = L"VVel:";
             // 
             // label15
             // 
             this->label15->AutoSize = true;
             this->label15->Location = System::Drawing::Point(7, 19);
             this->label15->Name = L"label15";
             this->label15->Size = System::Drawing::Size(33, 13);
             this->label15->TabIndex = 10;
             this->label15->Text = L"UVel:";
             // 
             // uvel
             // 
             this->uvel->Location = System::Drawing::Point(46, 16);
             this->uvel->Name = L"uvel";
             this->uvel->Size = System::Drawing::Size(74, 20);
             this->uvel->TabIndex = 9;
             this->uvel->TextChanged += gcnew System::EventHandler(this, &THPSLevel::uvel_TextChanged);
             this->uvel->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &THPSLevel::AllowNumbersOnly);
             // 
             // animated
             // 
             this->animated->AutoSize = true;
             this->animated->Location = System::Drawing::Point(3, 185);
             this->animated->Name = L"animated";
             this->animated->Size = System::Drawing::Size(70, 17);
             this->animated->TabIndex = 8;
             this->animated->Text = L"Animated";
             this->animated->UseVisualStyleBackColor = true;
             this->animated->CheckedChanged += gcnew System::EventHandler(this, &THPSLevel::animated_CheckedChanged);
             // 
             // BrowseTexture
             // 
             this->BrowseTexture->Enabled = false;
             this->BrowseTexture->Location = System::Drawing::Point(205, 181);
             this->BrowseTexture->Name = L"BrowseTexture";
             this->BrowseTexture->Size = System::Drawing::Size(75, 23);
             this->BrowseTexture->TabIndex = 6;
             this->BrowseTexture->Text = L"Browse...";
             this->BrowseTexture->UseVisualStyleBackColor = true;
             this->BrowseTexture->Click += gcnew System::EventHandler(this, &THPSLevel::BrowseTexture_Click);
             // 
             // MaterialList
             // 
             this->MaterialList->Location = System::Drawing::Point(6, 20);
             this->MaterialList->Name = L"MaterialList";
             this->MaterialList->Size = System::Drawing::Size(140, 87);
             this->MaterialList->TabIndex = 5;
             this->MaterialList->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &THPSLevel::matList_checkChanged);
             // 
             // doubled
             // 
             this->doubled->AutoSize = true;
             this->doubled->Location = System::Drawing::Point(3, 136);
             this->doubled->Name = L"doubled";
             this->doubled->Size = System::Drawing::Size(60, 17);
             this->doubled->TabIndex = 4;
             this->doubled->Text = L"2-sided";
             this->doubled->UseVisualStyleBackColor = true;
             this->doubled->CheckedChanged += gcnew System::EventHandler(this, &THPSLevel::doubled_CheckedChanged);
             // 
             // label14
             // 
             this->label14->AutoSize = true;
             this->label14->Location = System::Drawing::Point(3, 3);
             this->label14->Name = L"label14";
             this->label14->Size = System::Drawing::Size(52, 13);
             this->label14->TabIndex = 3;
             this->label14->Text = L"Materials:";
             // 
             // transparent
             // 
             this->transparent->AutoSize = true;
             this->transparent->Location = System::Drawing::Point(3, 113);
             this->transparent->Name = L"transparent";
             this->transparent->Size = System::Drawing::Size(79, 17);
             this->transparent->TabIndex = 1;
             this->transparent->Text = L"transparent";
             this->transparent->UseVisualStyleBackColor = true;
             this->transparent->CheckedChanged += gcnew System::EventHandler(this, &THPSLevel::transparent_CheckedChanged);
             // 
             // openFileDialog1
             // 
             this->openFileDialog1->FileName = L"openFileDialog1";
             // 
             // panel9
             // 
             this->panel9->Controls->Add(this->pictureBox1);
             this->panel9->Controls->Add(this->MaterialList);
             this->panel9->Controls->Add(this->transparent);
             this->panel9->Controls->Add(this->draworder);
             this->panel9->Controls->Add(this->label14);
             this->panel9->Controls->Add(this->animgroup);
             this->panel9->Controls->Add(this->doubled);
             this->panel9->Controls->Add(this->animated);
             this->panel9->Controls->Add(this->BrowseTexture);
             this->panel9->Controls->Add(this->uSlider);
             this->panel9->Controls->Add(this->vSlider);
             this->panel9->Dock = System::Windows::Forms::DockStyle::Fill;
             this->panel9->Location = System::Drawing::Point(3, 3);
             this->panel9->Name = L"panel9";
             this->panel9->Size = System::Drawing::Size(288, 285);
             this->panel9->TabIndex = 14;
             // 
             // pictureBox1
             // 
             this->pictureBox1->BackColor = System::Drawing::SystemColors::AppWorkspace;
             this->pictureBox1->ForeColor = System::Drawing::SystemColors::ControlText;
             this->pictureBox1->Location = System::Drawing::Point(151, 20);
             this->pictureBox1->Name = L"pictureBox1";
             this->pictureBox1->Size = System::Drawing::Size(133, 155);
             this->pictureBox1->TabIndex = 12;
             this->pictureBox1->Resize += gcnew System::EventHandler(this, &THPSLevel::ResizePictureWindow);
             //
             // uSlider
             //
             this->uSlider->Location = System::Drawing::Point(80, 110);
             this->uSlider->Size = System::Drawing::Size(20, 30);
             this->uSlider->Scroll += gcnew System::EventHandler(this, &THPSLevel::uSlider_Scroll);
             this->uSlider->Orientation = Orientation::Vertical;
             this->uSlider->Minimum = 1;
             this->uSlider->Maximum = 100;
             this->uSlider->TickFrequency = 10;
             this->uSlider->Value = 1;
             //
             // vSlider
             //
             this->vSlider->Location = System::Drawing::Point(100, 110);
             this->vSlider->Size = System::Drawing::Size(20, 30);
             this->vSlider->Scroll += gcnew System::EventHandler(this, &THPSLevel::vSlider_Scroll);
             this->vSlider->Orientation = Orientation::Vertical;
             this->vSlider->Minimum = 1;
             this->vSlider->Maximum = 100;
             this->vSlider->TickFrequency = 10;
             this->vSlider->Value = 1;
             // 
             // THPSLevel
             // 
             this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
             this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
             this->ClientSize = System::Drawing::Size(933, 576);
             this->Controls->Add(this->splitContainer1);
             this->Controls->Add(this->statusStrip1);
             this->Controls->Add(this->toolStrip1);
             this->Controls->Add(this->menuStrip1);
             this->ForeColor = System::Drawing::SystemColors::GrayText;
             this->KeyPreview = true;
             this->MainMenuStrip = this->menuStrip1;
             this->Name = L"THPSLevel";
             this->Text = L"THPS Level Editor";
             this->TransparencyKey = System::Drawing::Color::Magenta;
             this->Activated += gcnew System::EventHandler(this, &THPSLevel::GotFocus);
             this->Deactivate += gcnew System::EventHandler(this, &THPSLevel::LostFocus);
             this->Load += gcnew System::EventHandler(this, &THPSLevel::THPSLevel_Load);
             this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &THPSLevel::FKeyDown);
			       this->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &THPSLevel::FKeyPress);
             this->menuStrip1->ResumeLayout(false);
             this->menuStrip1->PerformLayout();
             this->toolStrip1->ResumeLayout(false);
             this->toolStrip1->PerformLayout();
             this->statusStrip1->ResumeLayout(false);
             this->statusStrip1->PerformLayout();
             this->splitContainer1->Panel1->ResumeLayout(false);
             this->splitContainer1->Panel1->PerformLayout();
             this->splitContainer1->Panel2->ResumeLayout(false);
             (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->splitContainer1))->EndInit();
             this->splitContainer1->ResumeLayout(false);
             this->splitContainer2->Panel1->ResumeLayout(false);
             this->splitContainer2->Panel1->PerformLayout();
             this->splitContainer2->Panel2->ResumeLayout(false);
             (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->splitContainer2))->EndInit();
             this->splitContainer2->ResumeLayout(false);
             this->panel7->ResumeLayout(false);
             this->tabControl1->ResumeLayout(false);
             this->tabPage1->ResumeLayout(false);
             this->tabPage2->ResumeLayout(false);
             this->tabPage2->PerformLayout();
             this->panel1->ResumeLayout(false);
             this->panel3->ResumeLayout(false);
             this->panel3->PerformLayout();
             (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBar1))->EndInit();
             this->tabPage3->ResumeLayout(false);
             this->groupBox1->ResumeLayout(false);
             this->panel5->ResumeLayout(false);
             this->panel5->PerformLayout();
             this->panel2->ResumeLayout(false);
             this->panel2->PerformLayout();
             (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDown1))->EndInit();
             this->panel4->ResumeLayout(false);
             this->panel4->PerformLayout();
             this->tabPage4->ResumeLayout(false);
             this->animgroup->ResumeLayout(false);
             this->animgroup->PerformLayout();
             this->groupBox3->ResumeLayout(false);
             this->groupBox3->PerformLayout();
             this->panel9->ResumeLayout(false);
             this->panel9->PerformLayout();
             this->ResumeLayout(false);
             this->PerformLayout();

           }
#pragma endregion




  private: System::Void THPSLevel_Load(System::Object^  sender, System::EventArgs^  e) {
             //SetFloatingPointRoundingToTruncate();
             InitRenderScene();
             //AllocConsole();
             printf("Welcome\n");
             //CreateThread(0,0,(LPTHREAD_START_ROUTINE)DrawFrame, 0, 0, 0);

           }
		   

           //private: System::Void DrawFrame(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e);
  private: System::Void tHPS2ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
           private: System::Void tHPS3ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void tHPS4ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);

  private: System::Void ResizeRenderWindow(System::Object^  sender, System::EventArgs^  e);
           private: System::Void uSlider_Scroll(Object^ /*sender*/, System::EventArgs^ /*e*/);
                    private: System::Void vSlider_Scroll(Object^ /*sender*/, System::EventArgs^ /*e*/);
  private: System::Void ResizePictureWindow(System::Object^  sender, System::EventArgs^  e);
  private: System::Void listBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
            private: System::Void toolStripButton8_Click(System::Object^  sender, System::EventArgs^  e);

  private: System::Void toolStripButton5_Click(System::Object^  sender, System::EventArgs^  e) {
             if(toolStripButton5->Checked)
             {
               //splitContainer1->SplitterDistance -= this->Width/3.5f;

               splitContainer2->Visible = true;
               splitContainer2->Enabled = true;
               this->splitContainer1->Panel2Collapsed = false;
             }
             else
             {
               splitContainer2->Visible = false;
               splitContainer2->Enabled = false;
               this->splitContainer1->Panel2Collapsed = true;

               //splitContainer1->SplitterDistance = this->Width;
             }
             extern bool forceRedraw;
             if(!forceRedraw)
               DrawFrame();
           }
  private: System::Void PropertyGrid1_PropertyValueChanged(System::Object ^ sender, System::Windows::Forms::PropertyValueChangedEventArgs ^ e)
           {
             MessageBox::Show(e->ChangedItem->ToString());
           }
  private: System::Void surfaceModeToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void railModeToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void objectModeToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void materialToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void collisionToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void objectsToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void tHPS3ToolStripMenuItem1_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void tHUG2ToolStripMenuItem1_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void loadToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
           {
             /*_fcloseall();

             FILE* f = fopen("C:\\text","rb");
             fseek(f, 0, SEEK_END);
             DWORD size = ftell(f);
             fseek(f, 0, SEEK_SET);
             BYTE* pFile = new BYTE[size];
             fread(pFile, size, 1, f);
             fclose(f);
             LoadTextures(pFile);
             delete [] pFile;

             f = fopen("C:\\lols","rb");
             fseek(f, 0, SEEK_END);
             size = ftell(f);
             fseek(f, 0, SEEK_SET);
             pFile = new BYTE[size];
             fread(pFile, size, 1, f);
             fclose(f);
             LoadState(pFile);
             delete [] pFile;*/
             bool file;
             char theFileName[MAX_PATH];
             OPENFILENAME ofn;
             ZeroMemory(&ofn, sizeof(ofn));
             ZeroMemory(theFileName, sizeof(theFileName));

             ofn.lStructSize = sizeof(ofn);
             ofn.hwndOwner = NULL;
             ofn.lpstrFilter = "Compressed Scene(*.zip)\0*.zip\0\0";
             ofn.lpstrFile = theFileName;
             ofn.nMaxFile = MAX_PATH;
             ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
             ofn.lpstrInitialDir = NULL;

             if (!GetOpenFileName(&ofn))
               file = false;
             else
               file = true;
             if (file)
             {
               LoadState(theFileName);
             }
           }
  private: System::Void saveToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
           {
             //TCHAR path[MAX_PATH] = "";

               TCHAR path[MAX_PATH] = "";
               //TCHAR rootDir[MAX_PATH] = "";
               BROWSEINFO bi = { 0 };
               LPMALLOC pMalloc = NULL;
               bi.hwndOwner = (HWND)(void*)this->Handle.ToPointer();
               /*GetCurrentDirectory(
               MAX_PATH,
               rootDir
               );*/
               if (SUCCEEDED(SHGetMalloc(&pMalloc)))
               {
                 //bi.pidlRoot = ConvertPathToLpItemIdList(rootDir);
                 //bi.pszDisplayName = folderName;
                 bi.lpszTitle = "Choose folder";
                 bi.ulFlags = BIF_NEWDIALOGSTYLE;
                 //bi.lpfn = Export;

                 LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

                 if (pidl != 0)
                 {
                   SHGetPathFromIDList(pidl, path);
                   /*char* name = scene->GetName();
                   DWORD pos = ((std::string)name).find_last_of("\\");

                   strcat(path, name + 1);*/

                   toolStripStatusLabel1->Text = "Saving Level: " + gcnew String(path);
                   this->Refresh();
                   SaveState(path);
                   _fcloseall();
                   toolStripStatusLabel1->Text = this->sceneName;
                   //ResetProgressbar();
                 }
                 pMalloc->Free(pidl);
                 pMalloc->Release();
               }
           }
  private: System::Void LostFocus(System::Object^  sender, System::EventArgs^  e);
  private: System::Void GotFocus(System::Object^  sender, System::EventArgs^  e);
  private: System::Void FKeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
  private: System::Void FKeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e);
  private: System::Void exitToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
             Close();
           }
  private: System::Void toolStripButton1_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void toolStripButton7_Click(System::Object^  sender, System::EventArgs^  e) {
             showSky = !showSky;
             extern bool forceRedraw;
             if(!forceRedraw)
               DrawFrame();
           }
  private: System::Void toolStripButton2_Click(System::Object^  sender, System::EventArgs^  e) {
             enableTransparency = !enableTransparency;
             extern bool forceRedraw;
             if(!forceRedraw)
               DrawFrame();
           }
  private: System::Void toolStripButton4_Click(System::Object^  sender, System::EventArgs^  e) {
             showRails = !showRails;
             extern bool forceRedraw;
             if(!forceRedraw)
               DrawFrame();
           }
  private: System::Void toolStripButton3_Click(System::Object^  sender, System::EventArgs^  e) {
             showSpawns = !showSpawns;
             extern bool forceRedraw;
             if(!forceRedraw)
               DrawFrame();
           }
  private: System::Void animated_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
  private: System::Void matList_checkChanged(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs ^  e);
  private: System::Void transparent_CheckedChanged(System::Object^  sender, System::EventArgs^  e);

           Material* GetSelectedMaterial();
  private: System::Void doubled_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
           System::Void draworder_TextChanged(System::Object^  sender, System::EventArgs^  e);
  private: System::Void uvel_TextChanged(System::Object^  sender, System::EventArgs^  e);
  private: System::Void vvel_TextChanged(System::Object^  sender, System::EventArgs^  e);
  private: System::Void water_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
  private: System::Void linear_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
  private: System::Void THPSLevel::button1_Click(System::Object^ sender, System::EventArgs^ e);
  private: System::Void THPSLevel::triggerButton1_Click(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
  private: System::Void THPSLevel::checkedListBox1_itemCheck(System::Object^  sender, System::Windows::Forms::ItemCheckEventArgs ^  e);

  private: System::Void BrowseTexture_Click(System::Object^  sender, System::EventArgs^  e) {
           }
  private: System::Void RenderGotFocus(System::Object^  sender, System::EventArgs^  e)
           {
             update=true;
           }
  private: System::Void RenderLostFocus(System::Object^  sender, System::EventArgs^  e)
           {
             update=false;
           }
  private: void AllowNumbersOnly(System::Object^ sender, System::Windows::Forms::KeyPressEventArgs^ e)
           {
             TextBox^ thisBox = safe_cast<TextBox^>(sender);
             if (!System::Char::IsControl(e->KeyChar)
               && !System::Char::IsDigit(e->KeyChar)
               && e->KeyChar != '.' && e->KeyChar != '-')
             {
               e->Handled = true;
             }

             // only allow one decimal point
             if (e->KeyChar == '.')
             {
               if(thisBox->Text->IndexOf('.') > -1)//(sender as TextBox).Text.IndexOf('.') > -1)
                 e->Handled = true;
             }
             else if(e->KeyChar == '-')
             {
               if(thisBox->Text->IndexOf('-') > -1)//(sender as TextBox).Text.IndexOf('.') > -1)
                 e->Handled = true;
             }
           }

};
}

