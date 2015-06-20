/*
 * VirtualDataViewCtrl.h
 *
 *  Created on: 19 ���� 2015 �.
 *      Author: ������
 */

#ifndef SRC_GUI_BASEDATAVIEWCTRL_H_
#define SRC_GUI_BASEDATAVIEWCTRL_H_

#include <wx/wx.h>
#include <wx/dataview.h>

#include "basedataviewmodel.h"

#include "settings.h"
#include "utils/slconfig.h"

template<class DataType>
class BaseDataViewCtrl: public wxDataViewCtrl {
public:
	BaseDataViewCtrl(wxString&, wxWindow*, wxWindowID);
	virtual ~BaseDataViewCtrl();

	bool AssociateModel(BaseDataViewModel<DataType>*);

	void Resort();
	void RefreshItem(const DataType&);
	bool ContainsItem(const DataType&);
	void Clear();
	const DataType* GetSelectedItem();

protected:
	virtual void LoadColumnProperties();
	virtual void SaveColumnProperties();

private:
	void OnSizeEvent(wxSizeEvent&);
	void AdjustColumnsWidth();

protected:
	BaseDataViewModel<DataType>* m_DataModel;
	wxString m_DataViewName;

	DECLARE_EVENT_TABLE()
};

template<class DataType>
inline void BaseDataViewCtrl<DataType>::AdjustColumnsWidth() {
	const int columnsCount = GetColumnCount();
	const int autoResizableColumnIndex = columnsCount - 1;

	if (autoResizableColumnIndex < 0) {
		return;
	}

	if (autoResizableColumnIndex >= columnsCount) {
		return;
	}

	int totalColumnsWidth = 0;
	for (int colIndex = 0; colIndex < columnsCount; ++colIndex) {
		if (colIndex == autoResizableColumnIndex) {
			continue;
		}
		const wxDataViewColumn* column = GetColumn(colIndex);
		totalColumnsWidth += column->GetWidth();
	}

	wxSize clientSize = GetClientSize();

	wxDataViewColumn* column = GetColumn(autoResizableColumnIndex);
	const int newSize = clientSize.GetWidth() - totalColumnsWidth;
	column->SetWidth(newSize);
}

BEGIN_EVENT_TABLE_TEMPLATE1(BaseDataViewCtrl, wxDataViewCtrl, DataType)
    EVT_SIZE(BaseDataViewCtrl::OnSizeEvent)
END_EVENT_TABLE()

template<class DataType>
BaseDataViewCtrl<DataType>::BaseDataViewCtrl(wxString& dataViewName, wxWindow* parent, wxWindowID id) : wxDataViewCtrl(parent, id) {
	m_DataModel = nullptr;
	m_DataViewName = dataViewName;
}

template<class DataType>
BaseDataViewCtrl<DataType>::~BaseDataViewCtrl() {
	SaveColumnProperties();
}

template<class DataType>
inline void BaseDataViewCtrl<DataType>::Resort() {
	wxASSERT(m_DataModel != nullptr);
	m_DataModel->Resort();
}

template<class DataType>
inline void BaseDataViewCtrl<DataType>::RefreshItem(const DataType& item) {
	wxASSERT(m_DataModel != nullptr);
	m_DataModel->UpdateItem(item);
}

template<class DataType>
inline bool BaseDataViewCtrl<DataType>::ContainsItem(const DataType& item) {
	wxASSERT(m_DataModel != nullptr);
	return m_DataModel->ContainsItem(item);
}

template<class DataType>
inline void BaseDataViewCtrl<DataType>::Clear() {
	wxASSERT(m_DataModel != nullptr);
	m_DataModel->Clear();
}

template<class DataType>
inline bool BaseDataViewCtrl<DataType>::AssociateModel(BaseDataViewModel<DataType>* model) {
	m_DataModel = model;

	return wxDataViewCtrl::AssociateModel(model);
}

template<class DataType>
inline const DataType* BaseDataViewCtrl<DataType>::GetSelectedItem() {
	wxDataViewItem item = GetSelection();

	if (item.IsOk() == false) {
		return nullptr;
	} else {
		return (static_cast<DataType*>(item.GetID()));
	}
}

template<class DataType>
inline void BaseDataViewCtrl<DataType>::LoadColumnProperties() {
	const int columnCount = GetColumnCount();

	for(int columnIndex = 0; columnIndex < columnCount; columnIndex++)
	{
		const int colWidth = sett().GetColumnWidth(m_DataViewName, columnIndex);
		if (colWidth < 0) { //Probably width is undefined yet?
			continue;
		}
		wxDataViewColumn* column = GetColumn(columnIndex);
		column->SetWidth(colWidth);
	}

	//Set up sorting column
	int sortingColumnIndex;
	cfg().Read(wxString(m_DataViewName + _T("/sorting_column")), &sortingColumnIndex, -1);
	if (sortingColumnIndex < 0) {
		return;
	}
	SetSortingColumnIndex(sortingColumnIndex);
	//Set up sorting order
	bool sortOrderAscending;
	cfg().Read(wxString(m_DataViewName + _T("/sorting_order")), &sortOrderAscending, true);
	wxDataViewColumn* column = GetColumn(sortingColumnIndex);
	column->SetSortOrder(sortOrderAscending);
}

template<class DataType>
inline void BaseDataViewCtrl<DataType>::SaveColumnProperties() {
	const int columnCount = GetColumnCount();

	for(int columnIndex = 0; columnIndex < columnCount; columnIndex++)
	{
		wxDataViewColumn* column = GetColumn(columnIndex);
		const int colWidth = column->GetWidth();
		sett().SetColumnWidth(m_DataViewName, columnIndex, colWidth);
	}

	//Save sorting column
	int sortingColumnIndex = GetSortingColumnIndex();
	cfg().Write(wxString(m_DataViewName + _T("/sorting_column")), sortingColumnIndex);
	//Save sorting order
	wxDataViewColumn* column = GetColumn(sortingColumnIndex);
	bool sortOrderAscending = column->IsSortOrderAscending();
	cfg().Write(wxString(m_DataViewName + _T("/sorting_order")), sortOrderAscending);
}

template<class DataType>
inline void BaseDataViewCtrl<DataType>::OnSizeEvent(wxSizeEvent& event) {
	AdjustColumnsWidth();
	event.Skip();
}

#endif /* SRC_GUI_BASEDATAVIEWCTRL_H_ */