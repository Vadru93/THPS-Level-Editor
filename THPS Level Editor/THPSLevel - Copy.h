#pragma once
#include "windows.h"
#include <d3d9.h>
#include "Th4Scene.h"
#include "Th2Scene.h"
#include <windowsx.h>
#include <Commdlg.h>
#include <d3dx9.h>
#include "shlobj.h"
#include "Camera.h"
#include "dinput.h"
#include "eventhandler.h"
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include "Debug.h"

void RenderLoop();

void ReleaseTextures();

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
  public:
    static bool rendering = false;
  public: System::Windows::Forms::ListBox^  listBox1;




  private: System::Windows::Forms::Timer^  timer1;
  private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;
  private: System::Windows::Forms::ToolStripButton^  toolStripButton7;
  private: System::Windows::Forms::ToolStripButton^  toolStripButton6;
  public: 
    static bool render = true;


    THPSLevel(void)
    {
      InitializeComponent();
      //
      //TODO: Add the constructor code here
      //
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
      CleanupRenderScene();
    }

  private:
    /// <summary>
    /// Required designer variable.
    /// </summary>

    void AttemptRecovery();

    void DrawFrame(System::Object ^,System::EventArgs ^);

    void InitRenderScene();

    void SetDeviceStates();

    void CreateManagedObjects();

    void DeleteManagedObjects();

    void SafeTurnOffRendering();

    bool LoadScene(char* path);


    void UnloadScene();
    void CleanupRenderScene();


    System::Windows::Forms::MenuStrip^ menuStrip1;
    System::Windows::Forms::ToolStripMenuItem^ fileToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ importToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ tHPS2ToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ tHPS3ToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ tHPS4ToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ exportToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ exitToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ helpToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ tHPS3ToolStripMenuItem1;
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
    System::Windows::Forms::ListBox^ listBox2;
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
    System::Windows::Forms::ComboBox^ comboBox2;


    System::Windows::Forms::GroupBox^ groupBox1;
    System::Windows::Forms::ToolStripDropDownButton^ toolStripDropDownButton1;
    System::Windows::Forms::ToolStripMenuItem^ objectsToolStripMenuItem;
    System::Windows::Forms::ToolStripMenuItem^ collisionToolStripMenuItem;





  private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator4;
  private: System::Windows::Forms::ToolStripButton^  toolStripButton5;
  private: System::Windows::Forms::Label^  label10;

  private: System::Windows::Forms::Panel^  panel2;
  private: System::Windows::Forms::NumericUpDown^  numericUpDown1;
  private: System::Windows::Forms::Label^  label3;
  private: System::Windows::Forms::CheckedListBox^  checkedListBox1;
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
  private: System::Windows::Forms::ToolStripButton^  toolStripButton4;
  private: System::Windows::Forms::ToolStripButton^  toolStripButton3;
  private: System::Windows::Forms::ToolStripLabel^  toolStripLabel1;
  private: System::Windows::Forms::Label^  label13;
  private: System::Windows::Forms::ToolStripProgressBar^  toolStripProgressBar1;
  private: System::Windows::Forms::ToolStripStatusLabel^  toolStripStatusLabel1;
  private: System::Windows::Forms::ToolStripStatusLabel^  toolStripStatusLabel2;
  public: System::Windows::Forms::Panel^  panel6;
  private: System::ComponentModel::IContainer^  components;


#pragma region Windows Form Designer generated code
           /// <summary>
           /// Required method for Designer support - do not modify
           /// the contents of this method with the code editor.
           /// </summary>
           void InitializeComponent(void)
           {
             this->components = (gcnew System::ComponentModel::Container());
             System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(THPSLevel::typeid));
             this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
             this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->importToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->tHPS2ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->tHPS3ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->tHPS4ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->exportToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
             this->tHPS3ToolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
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
             this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
             this->toolStripProgressBar1 = (gcnew System::Windows::Forms::ToolStripProgressBar());
             this->toolStripStatusLabel1 = (gcnew System::Windows::Forms::ToolStripStatusLabel());
             this->toolStripStatusLabel2 = (gcnew System::Windows::Forms::ToolStripStatusLabel());
             this->splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
             this->panel6 = (gcnew System::Windows::Forms::Panel());
             this->splitContainer2 = (gcnew System::Windows::Forms::SplitContainer());
             this->listBox1 = (gcnew System::Windows::Forms::ListBox());
             this->label1 = (gcnew System::Windows::Forms::Label());
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
             this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
             this->label8 = (gcnew System::Windows::Forms::Label());
             this->button4 = (gcnew System::Windows::Forms::Button());
             this->button3 = (gcnew System::Windows::Forms::Button());
             this->label7 = (gcnew System::Windows::Forms::Label());
             this->textBox3 = (gcnew System::Windows::Forms::TextBox());
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
             this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
             this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
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
             this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->importToolStripMenuItem, 
               this->exportToolStripMenuItem, this->exitToolStripMenuItem});
             this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
             this->fileToolStripMenuItem->Size = System::Drawing::Size(35, 20);
             this->fileToolStripMenuItem->Text = L"&File";
             // 
             // importToolStripMenuItem
             // 
             this->importToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->tHPS2ToolStripMenuItem, 
               this->tHPS3ToolStripMenuItem, this->tHPS4ToolStripMenuItem});
             this->importToolStripMenuItem->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"importToolStripMenuItem.Image")));
             this->importToolStripMenuItem->Name = L"importToolStripMenuItem";
             this->importToolStripMenuItem->Size = System::Drawing::Size(106, 22);
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
             // 
             // tHPS4ToolStripMenuItem
             // 
             this->tHPS4ToolStripMenuItem->Name = L"tHPS4ToolStripMenuItem";
             this->tHPS4ToolStripMenuItem->Size = System::Drawing::Size(117, 22);
             this->tHPS4ToolStripMenuItem->Text = L"THPS4...";
             // 
             // exportToolStripMenuItem
             // 
             this->exportToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->tHPS3ToolStripMenuItem1, 
               this->oBJToolStripMenuItem});
             this->exportToolStripMenuItem->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"exportToolStripMenuItem.Image")));
             this->exportToolStripMenuItem->Name = L"exportToolStripMenuItem";
             this->exportToolStripMenuItem->Size = System::Drawing::Size(106, 22);
             this->exportToolStripMenuItem->Text = L"Export";
             // 
             // tHPS3ToolStripMenuItem1
             // 
             this->tHPS3ToolStripMenuItem1->Name = L"tHPS3ToolStripMenuItem1";
             this->tHPS3ToolStripMenuItem1->Size = System::Drawing::Size(117, 22);
             this->tHPS3ToolStripMenuItem1->Text = L"THPS3...";
             this->tHPS3ToolStripMenuItem1->Click += gcnew System::EventHandler(this, &THPSLevel::tHPS3ToolStripMenuItem1_Click);
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
             this->exitToolStripMenuItem->Size = System::Drawing::Size(106, 22);
             this->exitToolStripMenuItem->Text = L"Exit";
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
             this->toolStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(12) {this->toolStripSplitButton1, 
               this->toolStripSeparator1, this->toolStripDropDownButton1, this->toolStripButton1, this->toolStripButton7, this->toolStripButton2, 
               this->toolStripButton4, this->toolStripButton3, this->toolStripButton6, this->toolStripSeparator4, this->toolStripLabel1, this->toolStripButton5});
             this->toolStrip1->Location = System::Drawing::Point(0, 24);
             this->toolStrip1->Name = L"toolStrip1";
             this->toolStrip1->Size = System::Drawing::Size(933, 25);
             this->toolStrip1->TabIndex = 2;
             this->toolStrip1->Text = L"toolStrip1";
             // 
             // toolStripSplitButton1
             // 
             this->toolStripSplitButton1->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->objectModeToolStripMenuItem, 
               this->surfaceModeToolStripMenuItem, this->railModeToolStripMenuItem});
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
             this->railModeToolStripMenuItem->Text = L"Rail Mode";
             this->railModeToolStripMenuItem->Click += gcnew System::EventHandler(this, &THPSLevel::railModeToolStripMenuItem_Click);
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
             // 
             // toolStripButton4
             // 
             this->toolStripButton4->CheckOnClick = true;
             this->toolStripButton4->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
             this->toolStripButton4->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripButton4.Image")));
             this->toolStripButton4->ImageTransparentColor = System::Drawing::Color::Magenta;
             this->toolStripButton4->Name = L"toolStripButton4";
             this->toolStripButton4->Size = System::Drawing::Size(23, 22);
             this->toolStripButton4->Text = L"Show Rails";
             this->toolStripButton4->ToolTipText = L"Show Rails";
             // 
             // toolStripButton3
             // 
             this->toolStripButton3->CheckOnClick = true;
             this->toolStripButton3->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
             this->toolStripButton3->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"toolStripButton3.Image")));
             this->toolStripButton3->ImageTransparentColor = System::Drawing::Color::Magenta;
             this->toolStripButton3->Name = L"toolStripButton3";
             this->toolStripButton3->Size = System::Drawing::Size(23, 22);
             this->toolStripButton3->Text = L"Show Respawns";
             this->toolStripButton3->ToolTipText = L"Show Respawns";
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
             this->toolStripStatusLabel1->Size = System::Drawing::Size(99, 17);
             this->toolStripStatusLabel1->Text = L"Loading level Rio...";
             // 
             // toolStripStatusLabel2
             // 
             this->toolStripStatusLabel2->Name = L"toolStripStatusLabel2";
             this->toolStripStatusLabel2->Size = System::Drawing::Size(717, 17);
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
             this->splitContainer1->Panel1->Padding = System::Windows::Forms::Padding(8);
             // 
             // splitContainer1.Panel2
             // 
             this->splitContainer1->Panel2->Controls->Add(this->splitContainer2);
             this->splitContainer1->Size = System::Drawing::Size(933, 505);
             this->splitContainer1->SplitterDistance = 625;
             this->splitContainer1->TabIndex = 4;
             // 
             // panel6
             // 
             this->panel6->AutoSize = true;
             this->panel6->BackColor = System::Drawing::SystemColors::AppWorkspace;
             this->panel6->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
             this->panel6->Dock = System::Windows::Forms::DockStyle::Fill;
             this->panel6->ForeColor = System::Drawing::SystemColors::ControlText;
             this->panel6->Location = System::Drawing::Point(8, 8);
             this->panel6->Name = L"panel6";
             this->panel6->Size = System::Drawing::Size(609, 489);
             this->panel6->TabIndex = 0;
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
             this->splitContainer2->Panel1->Padding = System::Windows::Forms::Padding(8);
             // 
             // splitContainer2.Panel2
             // 
             this->splitContainer2->Panel2->Controls->Add(this->tabControl1);
             this->splitContainer2->Size = System::Drawing::Size(304, 505);
             this->splitContainer2->SplitterDistance = 175;
             this->splitContainer2->TabIndex = 0;
             // 
             // listBox1
             // 
             this->listBox1->Dock = System::Windows::Forms::DockStyle::Fill;
             this->listBox1->FormattingEnabled = true;
             this->listBox1->Location = System::Drawing::Point(8, 21);
             this->listBox1->Name = L"listBox1";
             this->listBox1->Size = System::Drawing::Size(288, 146);
             this->listBox1->TabIndex = 1;
             this->listBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &THPSLevel::listBox1_SelectedIndexChanged);
             // 
             // label1
             // 
             this->label1->AutoSize = true;
             this->label1->Dock = System::Windows::Forms::DockStyle::Top;
             this->label1->Location = System::Drawing::Point(8, 8);
             this->label1->Margin = System::Windows::Forms::Padding(3, 0, 3, 3);
             this->label1->Name = L"label1";
             this->label1->Size = System::Drawing::Size(60, 13);
             this->label1->TabIndex = 0;
             this->label1->Text = L"All Objects:";
             // 
             // tabControl1
             // 
             this->tabControl1->Controls->Add(this->tabPage1);
             this->tabControl1->Controls->Add(this->tabPage2);
             this->tabControl1->Controls->Add(this->tabPage3);
             this->tabControl1->Dock = System::Windows::Forms::DockStyle::Fill;
             this->tabControl1->Location = System::Drawing::Point(0, 0);
             this->tabControl1->Name = L"tabControl1";
             this->tabControl1->SelectedIndex = 0;
             this->tabControl1->Size = System::Drawing::Size(304, 326);
             this->tabControl1->TabIndex = 0;
             // 
             // tabPage1
             // 
             this->tabPage1->Controls->Add(this->propertyGrid1);
             this->tabPage1->ForeColor = System::Drawing::SystemColors::ControlText;
             this->tabPage1->Location = System::Drawing::Point(4, 22);
             this->tabPage1->Name = L"tabPage1";
             this->tabPage1->Padding = System::Windows::Forms::Padding(8);
             this->tabPage1->Size = System::Drawing::Size(296, 300);
             this->tabPage1->TabIndex = 0;
             this->tabPage1->Text = L"Properties";
             this->tabPage1->UseVisualStyleBackColor = true;
             // 
             // propertyGrid1
             // 
             this->propertyGrid1->BackColor = System::Drawing::SystemColors::Window;
             this->propertyGrid1->Dock = System::Windows::Forms::DockStyle::Fill;
             this->propertyGrid1->Location = System::Drawing::Point(8, 8);
             this->propertyGrid1->Name = L"propertyGrid1";
             this->propertyGrid1->Size = System::Drawing::Size(280, 284);
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
             this->tabPage2->Size = System::Drawing::Size(296, 300);
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
             this->listBox2->Size = System::Drawing::Size(200, 115);
             this->listBox2->TabIndex = 3;
             // 
             // panel1
             // 
             this->panel1->Controls->Add(this->button2);
             this->panel1->Controls->Add(this->button1);
             this->panel1->Dock = System::Windows::Forms::DockStyle::Right;
             this->panel1->Location = System::Drawing::Point(208, 21);
             this->panel1->Name = L"panel1";
             this->panel1->Padding = System::Windows::Forms::Padding(5, 0, 0, 0);
             this->panel1->Size = System::Drawing::Size(80, 115);
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
             this->panel3->Location = System::Drawing::Point(8, 136);
             this->panel3->Name = L"panel3";
             this->panel3->Size = System::Drawing::Size(280, 156);
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
             // 
             // comboBox1
             // 
             this->comboBox1->FormattingEnabled = true;
             this->comboBox1->Location = System::Drawing::Point(81, 122);
             this->comboBox1->Name = L"comboBox1";
             this->comboBox1->Size = System::Drawing::Size(188, 21);
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
             this->tabPage3->Size = System::Drawing::Size(296, 300);
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
             this->groupBox1->Size = System::Drawing::Size(280, 176);
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
             this->panel5->Size = System::Drawing::Size(296, 22);
             this->panel5->TabIndex = 2;
             // 
             // comboBox3
             // 
             this->comboBox3->Dock = System::Windows::Forms::DockStyle::Fill;
             this->comboBox3->FormattingEnabled = true;
             this->comboBox3->Location = System::Drawing::Point(88, 0);
             this->comboBox3->Name = L"comboBox3";
             this->comboBox3->Size = System::Drawing::Size(208, 21);
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
             this->checkedListBox1->Size = System::Drawing::Size(278, 94);
             this->checkedListBox1->TabIndex = 1;
             // 
             // panel2
             // 
             this->panel2->Controls->Add(this->numericUpDown1);
             this->panel2->Controls->Add(this->label3);
             this->panel2->Location = System::Drawing::Point(0, 19);
             this->panel2->Name = L"panel2";
             this->panel2->Size = System::Drawing::Size(296, 22);
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
             this->panel4->Controls->Add(this->label9);
             this->panel4->Controls->Add(this->comboBox2);
             this->panel4->Dock = System::Windows::Forms::DockStyle::Top;
             this->panel4->Location = System::Drawing::Point(8, 8);
             this->panel4->Name = L"panel4";
             this->panel4->Size = System::Drawing::Size(280, 26);
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
             // timer1
             // 
             this->timer1->Enabled = true;
             this->timer1->Interval = 15;
             this->timer1->Tick += gcnew System::EventHandler(this, &THPSLevel::DrawFrame);
             // 
             // openFileDialog1
             // 
             this->openFileDialog1->FileName = L"openFileDialog1";
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
             this->ResumeLayout(false);
             this->PerformLayout();

           }
#pragma endregion



  private: System::Void THPSLevel_Load(System::Object^  sender, System::EventArgs^  e) {
             InitRenderScene();

             //CreateThread(0,0,(LPTHREAD_START_ROUTINE)RenderLoop, 0, 0, 0);

           }
           //private: System::Void DrawFrame(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e);
  private: System::Void tHPS2ToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);

  private: System::Void ResizeRenderWindow(System::Object^  sender, System::EventArgs^  e);
  private: System::Void listBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);

  private: System::Void toolStripButton5_Click(System::Object^  sender, System::EventArgs^  e) {
             if(toolStripButton5->Checked)
             {
               splitContainer1->SplitterDistance -= this->Width/3.5;

               splitContainer2->Visible = true;
               splitContainer2->Enabled = true;
             }
             else
             {
               splitContainer2->Visible = false;
               splitContainer2->Enabled = false;

               splitContainer1->SplitterDistance = this->Width;
             }
           }

  private: System::Void surfaceModeToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void railModeToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void objectModeToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void collisionToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void objectsToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void tHPS3ToolStripMenuItem1_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void LostFocus(System::Object^  sender, System::EventArgs^  e);
  private: System::Void GotFocus(System::Object^  sender, System::EventArgs^  e);
  private: System::Void FKeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
  };
}

