Attribute VB_Name = "Module1"
Sub Macro1()
Attribute Macro1.VB_Description = "Macro recorded 2002.02.09 by agris"
Attribute Macro1.VB_ProcData.VB_Invoke_Func = " \n14"
'
' Macro1 Macro
' Macro recorded 2002.02.09 by agris
'
    fileToOpen = Application.GetOpenFilename()
    If (fileToOpen = 0) Then Exit Sub
    Workbooks.OpenText Filename:=fileToOpen, _
        Origin:=xlWindows, StartRow:=1, DataType:=xlDelimited, TextQualifier:= _
        xlDoubleQuote, ConsecutiveDelimiter:=False, Tab:=False, Semicolon:=True _
        , Comma:=True, Space:=False, Other:=False, FieldInfo:=Array(Array(1, 2), _
        Array(2, 2), Array(3, 3), Array(4, 1), Array(5, 2))
    Columns("A:A").ColumnWidth = 20
    Columns("B:B").ColumnWidth = 20
    Columns("C:C").ColumnWidth = 15
    Columns("D:D").ColumnWidth = 15
    Columns("E:E").ColumnWidth = 100
    Rows(1).Insert
    Range("A1").Select
    ActiveCell.FormulaR1C1 = "Code"
    Range("B1").Select
    ActiveCell.FormulaR1C1 = "Serial"
    Range("C1").Select
    ActiveCell.FormulaR1C1 = "Date(MMDDYY)"
    Range("D1").Select
    ActiveCell.FormulaR1C1 = "Time"
    Range("E1").Select
    ActiveCell.FormulaR1C1 = "Description"

End Sub
