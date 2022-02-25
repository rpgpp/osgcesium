#ifndef _OC_SHP_DRAWER_HELPER_H__
#define _OC_SHP_DRAWER_HELPER_H__

#include "ShpDrawer.h"

namespace OC
{
	class ShpCol
	{
	public:
		ShpCol(float width, String text = StringUtil::BLANK)
			:mWidth(width)
			, mText(text)
		{

		}

		bool	mFill = false;
		bool	mMerged = false;
		float	mWidth;
		String	mText;

	};
	typedef std::vector<ShpCol> ShpColumnList;

	class ShpRow
	{
	public:
		ShpRow(float height = 0.0)
			:mHeight(height)
		{

		}
		int  colSize() { return (int)mColumnList.size(); }
		void pushCol(ShpCol col)
		{
			mColumnList.push_back(col);
		}
		int				mRowID = 0;
		float			mHeight;
		ShpColumnList	mColumnList;
	};
	typedef std::vector<ShpRow> ShpRowList;

	class ShpExtraData
	{
	public:
		ShpExtraData()
		{}
		bool isValid()
		{
			return mLeft >= 0 && mLeft <= mRight
				&& mTop >= 0
				&& mTop <= mBottom && !mRows.empty();
		}
		void addRow(ShpRow row)
		{
			row.mRowID = (int)mRows.size();
			mRows.push_back(row);
		}
		int				mLeft = 0, mBottom = 0, mRight = 0, mTop = 0;
		ShpRowList		mRows;
	};

	class ShpDrawerHelper
	{
	public:
		void readFile(OC::Json::Value& root)
		{
			mGridWidth = JsonPackage::getDouble(root, "columnWidth", 2.0);
			mGridHeight = JsonPackage::getDouble(root, "columnHeight", 1.0);
			float maxColumnCount = JsonPackage::getDouble(root, "maxColumnCount", 10);
			mTableWidth = maxColumnCount * mGridWidth;

			auto parseRow = [this](auto& r)
			{
				float rowHeight = JsonPackage::getDouble(r, "rowHeight", 1.0);
				OC::Json::Value cols = r["column"];
				ShpRow row(mGridHeight * rowHeight);
				for (auto c : cols)
				{
					String text = JsonPackage::getString(c, "name");
					text = osgDB::convertStringFromUTF8toCurrentCodePage(text);
					float width = JsonPackage::getDouble(c, "width");
					ShpCol col(mGridWidth * width, text);
					col.mMerged = JsonPackage::getBool(c, "merge");
					col.mFill = JsonPackage::getBool(c, "fill");
					row.pushCol(col);
				}
				return row;
			};

			OC::Json::Value data = root["data"];
			for (auto& r : data)
			{
				ShpRow row = parseRow(r);
				addRow(row);
			}

			//extra table
			OC::Json::Value extraData = root["extraData"];
			if (!extraData.isNull())
			{
				mExtraData.mLeft = JsonPackage::getInt(extraData, "left");
				mExtraData.mBottom = JsonPackage::getInt(extraData, "bottom");
				mExtraData.mRight = JsonPackage::getInt(extraData, "right");
				mExtraData.mTop = JsonPackage::getInt(extraData, "top");
				OC::Json::Value data = extraData["data"];
				for (auto r : data)
				{
					ShpRow row = parseRow(r);
					mExtraData.addRow(row);
				}
			}
		}

		void readFile(istream& is)
		{
			OC::Json::Value root;
			OC::Json::Reader reader;
			if (reader.parse(is, root))
			{
				readFile(root);
			}
		}
		
		void readFile(String filename)
		{
			std::ifstream ifs(filename.c_str());
			if (ifs)
			{
				readFile(ifs);
				ifs.close();
			}
		}

		std::vector<ShpRow*> findMergeRow(ShpRow& row)
		{
			std::vector<ShpRow*> rows;
			for (int i = row.mRowID + 1; i < (int)mRows.size();i++)
			{
				ShpRow* row2 = &mRows[i];
				if(row.colSize() != row2->colSize())
				{
					break;
				}
				rows.push_back(row2);
			}
			return rows;
		}

		void drawRow(ShpRow& row1)
		{
			std::vector<ShpRow*> mergeRows = findMergeRow(row1);

			float rowHeight = row1.mHeight;

			ShpColumnList& colList1 = row1.mColumnList;
			
			int colCount1 = (int)colList1.size();

			float offsetX = originPos.x;

			bool testMergeGrid = !mergeRows.empty();
			bool hasMegerd = false;
			for (int i = 0; i < colCount1; i++)
			{
				gridPPI().y = rowHeight;
				ShpCol col = colList1[i];
				String text = col.mText;

				if (testMergeGrid && col.mMerged)
				{
					for (auto mc : mergeRows)
					{
						ShpRow& row2 = *mc;
						ShpCol& col2 = row2.mColumnList[i];
						if (text == col2.mText && col2.mMerged)
						{
							gridPPI().y += row2.mHeight;
							col2.mText = "";
							hasMegerd = true;
						}
					}
				}

				// grid left
				moveTo(offsetX, currentTablePos.y);
				lineTo(offsetX, currentTablePos.y - currentGridPPi.y);
				
				float colWidth = col.mWidth;
				if (i == colCount1 - 1)
				{
					colWidth = mTableWidth - offsetX + originPos.x;
					// grid right
					moveTo(offsetX + colWidth, currentTablePos.y);
					lineTo(offsetX + colWidth, currentTablePos.y - currentGridPPi.y);
				}

				if (col.mFill)
				{
					drawRect(colWidth, gridPPI().y);
					mDrawer.pushPolygonName(text);
				}
				else if(!text.empty())
				{
					mDrawer.pushPoint(offsetX, currentTablePos.y - currentGridPPi.y * 0.5);
					mDrawer.pushPointName(text);
				}

				// grid bottom
				moveTo(offsetX, currentTablePos.y - gridPPI().y);
				lineTo(offsetX + colWidth, currentTablePos.y - gridPPI().y);

				offsetX += colWidth;
			}

			gridPPI().y = rowHeight;
			tablePos().y -= gridPPI().y;
		}

		void output(String dir)
		{
			drawTable();
			float height = tablePos().y -= gridPPI().y;
			mDrawer.setExtent(0, height, mTableWidth, 0.0);
			mDrawer.output(dir);
		}

		void moveTo(CVector2 pos)
		{
			currentPos.x = pos.x;
			currentPos.y = pos.y;
		}

		void moveTo(float x, float y)
		{
			currentPos.x = x;
			currentPos.y = y;
		}

		void lineTo(float x, float y)
		{
			osgEarth::LineString* geom = mDrawer.pushLine();
			geom->push_back(currentPos.x, currentPos.y);
			geom->push_back(x, y);
		}

		void drawRect(float width, float height)
		{
			osgEarth::Polygon* geom = mDrawer.pushPolygon();
			geom->push_back(currentPos.x, currentPos.y);
			geom->push_back(currentPos.x + width, currentPos.y);
			geom->push_back(currentPos.x + width, currentPos.y - height);
			geom->push_back(currentPos.x, currentPos.y - height);
			geom->push_back(currentPos.x, currentPos.y);
		}
		CVector2& tablePos() { return currentTablePos; }
		CVector2& gridPPI() { return currentGridPPi; }

		void drawRowLine()
		{
			moveTo(tablePos());
			lineTo(mTableWidth, tablePos().y);
		}
		
		void addRows(ShpRowList rows)
		{
			for (auto& r : rows)
			{
				r.mHeight *= mGridHeight;
				for (auto& c : r.mColumnList)
				{
					c.mWidth *= mGridWidth;
				}
				addRow(r);
			}
		}

		void addRow(ShpRow row)
		{
			row.mRowID = (int)mRows.size();
			mRows.push_back(row);
		}

		void drawTable()
		{
			tablePos().x = 0;
			tablePos().y = 0;
			drawRowLine();
			for (auto& row : mRows)
			{
				drawRow(row);
			}

			if (mExtraData.isValid())
			{
				tablePos().x = 0;
				// test
				tablePos().y = 0;
				int top = mExtraData.mTop;
				for (int i = 0; i < mExtraData.mTop; i++)
				{
					tablePos().y -= mRows[i].mHeight;
				}
				for (int i = 0; i < mExtraData.mLeft; i++)
				{
					tablePos().x += mRows[top].mColumnList[i].mWidth;
				}

				originPos.x = tablePos().x;
				mTableWidth = 0.0;

				for (int i = mExtraData.mLeft; i < mExtraData.mRight + 1; i++)
				{
					mTableWidth += mRows[top].mColumnList[i].mWidth;
				}

				mRows = mExtraData.mRows;
				for (auto& row : mRows)
				{
					drawRow(row);
				}
			}
		}

		float getGridHeight()
		{
			return mGridHeight;
		}
	protected:
		float							mGridWidth = 2.0;
		float							mGridHeight = 1.0;
		float							mGridNum = 10;
		float							mTableWidth = 20.0;
		CShpDrawer						mDrawer;
		ShpRowList						mRows;
		ShpExtraData					mExtraData;
		CVector2 originPos = CVector2::ZERO;
		CVector2 currentPos = CVector2::ZERO;
		CVector2 currentTablePos = CVector2::ZERO;
		CVector2 currentGridPPi = CVector2(mGridWidth, mGridHeight);
};
}


#endif // !_OC_SHP_DRAWER_H__




