/***********************************************************
 * File generated by the HALCON-Compiler hcomp version 12.0
 * Usage: Interface to C++
 *
 * Software by: MVTec Software GmbH, www.mvtec.com
 ***********************************************************/


#ifndef HCPP_HTEXTMODEL
#define HCPP_HTEXTMODEL

namespace HalconCpp
{

// Represents an instance of a text model for text segmentations.
class LIntExport HTextModel : public HToolBase
{

public:

  // Copy constructor
  HTextModel(const HTextModel& source) : HToolBase(source) {}

  // Create HTextModel from handle, taking ownership
  explicit HTextModel(Hlong handle);

  // Set new handle, taking ownership
  void SetHandle(Hlong handle);



/*****************************************************************************
 * Operator-based class constructors
 *****************************************************************************/

  // create_text_model_reader: Create a text model.
  explicit HTextModel(const HString& Mode, const HTuple& OCRClassifierMLP);

  // create_text_model_reader: Create a text model.
  explicit HTextModel(const HString& Mode, const HString& OCRClassifierMLP);

  // create_text_model_reader: Create a text model.
  explicit HTextModel(const char* Mode, const char* OCRClassifierMLP);

  // create_text_model: Create a text model.
  explicit HTextModel();




  /***************************************************************************
   * Operators                                                               *
   ***************************************************************************/

  // Find text in an image.
  HTextResult FindText(const HImage& Image) const;

  // Query parameters of a text model.
  HTuple GetTextModelParam(const HString& GenParamName) const;

  // Query parameters of a text model.
  HTuple GetTextModelParam(const char* GenParamName) const;

  // Set parameters of a text model.
  void SetTextModelParam(const HTuple& GenParamName, const HTuple& GenParamValue) const;

  // Set parameters of a text model.
  void SetTextModelParam(const HString& GenParamName, Hlong GenParamValue) const;

  // Set parameters of a text model.
  void SetTextModelParam(const char* GenParamName, Hlong GenParamValue) const;

  // Create a text model.
  void CreateTextModelReader(const HString& Mode, const HTuple& OCRClassifierMLP);

  // Create a text model.
  void CreateTextModelReader(const HString& Mode, const HString& OCRClassifierMLP);

  // Create a text model.
  void CreateTextModelReader(const char* Mode, const char* OCRClassifierMLP);

  // Create a text model.
  void CreateTextModel();

};

// forward declarations and types for internal array implementation

template<class T> class HSmartPtr;
template<class T> class HToolArrayRef;

typedef HToolArrayRef<HTextModel> HTextModelArrayRef;
typedef HSmartPtr< HTextModelArrayRef > HTextModelArrayPtr;


// Represents multiple tool instances
class LIntExport HTextModelArray : public HToolArray
{

public:

  // Create empty array
  HTextModelArray();

  // Create array from native array of tool instances
  HTextModelArray(HTextModel* classes, Hlong length);

  // Copy constructor
  HTextModelArray(const HTextModelArray &tool_array);

  // Destructor
  virtual ~HTextModelArray();

  // Assignment operator
  HTextModelArray &operator=(const HTextModelArray &tool_array);

  // Clears array and all tool instances
  virtual void Clear();

  // Get array of native tool instances
  const HTextModel* Tools() const;

  // Get number of tools
  virtual Hlong Length() const;

  // Create tool array from tuple of handles
  virtual void SetFromTuple(const HTuple& handles);

  // Get tuple of handles for tool array
  virtual HTuple ConvertToTuple() const;

protected:

// Smart pointer to internal data container
   HTextModelArrayPtr *mArrayPtr;
};

}

#endif
