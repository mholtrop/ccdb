import collections

__author__ = 'DmitryRa'

import datetime
import posixpath

import sqlalchemy
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.schema import Column, ForeignKey
from sqlalchemy.types import Integer, String, Text, DateTime, Enum
from sqlalchemy.orm import sessionmaker, reconstructor
from sqlalchemy.orm import relationship, backref
from sqlalchemy.sql.expression import desc


print sqlalchemy.__version__
Base = declarative_base()

#This thing separates cells in data blob
blob_delimiter = "|"

# if cell of data table is a string and the string already contains blob_delimiter
# we have to encode blob_delimiter to blob_delimiter_replace on data write and decode it bach on data read
blob_delimiter_replacement = "&delimiter;"

class Directory(Base):
    __tablename__ = 'directories'
    id = Column(Integer, primary_key=True)
    name = Column(String(255))
    comment = Column(Text)
    created = Column(DateTime, default = datetime.datetime.now)
    modified = Column(DateTime, default = datetime.datetime.now, onupdate = datetime.datetime.now)
    parent_id = Column('parentId', Integer)

    def __init__(self):
        self.path = ""
        self.parent_dir = None
        self.sub_dirs = []

    @reconstructor
    def on_load_init(self):
        self.path = ""
        self.parent_dir = None
        self.sub_dirs = []

    def __repr__(self):
        return "<Directory {0} '{1}'>".format(self.id, self.name)



class TypeTable(Base):
    __tablename__ = 'typeTables'
    id = Column(Integer, primary_key=True)
    name = Column(String(255))
    comment = Column("comments", Text)
    created = Column(DateTime, default = datetime.datetime.now)
    modified = Column(DateTime, default = datetime.datetime.now, onupdate = datetime.datetime.now)
    parent_dir_id = Column('directoryId',Integer, ForeignKey('directories.id'))
    parent_dir = relationship("Directory", backref=backref('type_tables', order_by=id))
    constant_sets = relationship("ConstantSet", backref=backref('type_table'))
    columns = relationship("TypeTableColumn", order_by="TypeTableColumn.order", cascade="all, delete, delete-orphan", backref=backref("type_table") )
    rows_count = Column('nRows',Integer)
    _columns_count = Column('nColumns',Integer)

    @property
    def path(self):
        """
        :return: full path of the table
        :rtype: str
        """
        return posixpath.join(self.parent_dir.path, self.name)

    def __repr__(self):
        return "<TypeTable {0} '{1}'>".format(self.id, self.name)



class TypeTableColumn(Base):
    __tablename__ = 'columns'
    id = Column(Integer, primary_key=True)
    name = Column(String(255))
    comment = Column(Text)
    created = Column(DateTime, default = datetime.datetime.now)
    modified = Column(DateTime, default = datetime.datetime.now, onupdate = datetime.datetime.now)
    order = Column(Integer)
    column_type = Column('columnType', Enum('int', 'uint','long','ulong','double','string','bool'))
    type_table_id = Column('typeId',Integer, ForeignKey('typeTables.id'))


    @property
    def path(self):
        return posixpath.join(self.parent_dir.path, self.name)

    def __repr__(self):
        return "<TypeTableColumn '{0}'>".format(self.name)



class ConstantSet(Base):
    __tablename__ = 'constantSets'
    id = Column(Integer, primary_key=True)
    _vault = Column('vault', Text)
    created = Column(DateTime, default = datetime.datetime.now)
    modified = Column(DateTime, default = datetime.datetime.now, onupdate = datetime.datetime.now)
    assignment = relationship("Assignment", uselist=False, back_populates="constant_set")
    type_table_id = Column('constantTypeId',Integer, ForeignKey('typeTables.id'))

    @property
    def vault(self):
        """
        Text-blob with data as it is presented in database
        :return: string with text-blob from db
        :rtype:  string
        """
        return self._vault

    @property
    def data_list(self):
        return blob_to_list(self._vault)

    @data_list.setter
    def data_list(self, list):
        self._vault = list_to_blob(list)

    @property
    def data_table(self):
        return list_to_table(self.data_list, self.type_table._columns_count)

    @data_table.setter
    def data_table(self, data):
        self.data_list = list(gen_flatten_data(data))


    def __repr__(self):
        return "<ConstantSet '{0}'>".format(self.id)


class Assignment(Base):
    __tablename__ = 'assignments'

    id = Column(Integer, primary_key=True)
    created = Column(DateTime, default = datetime.datetime.now)
    modified = Column(DateTime, default = datetime.datetime.now, onupdate = datetime.datetime.now)
    constant_set_id = Column('constantSetId', Integer, ForeignKey('constantSets.id'))
    constant_set = relationship("ConstantSet", uselist=False, back_populates="assignment")
    run_range_id = Column('runRangeId',Integer, ForeignKey('runRanges.id'))
    run_range = relationship("RunRange", backref=backref('assignments'))
    variation_id = Column('variationId',Integer, ForeignKey('variations.id'))
    variation = relationship("Variation", backref=backref('assignments'))

    def __repr__(self):
        return "<Assignment '{0}'>".format(self.id)

    def print_deps(self):
        print " ASSIGNMENT: " + repr(self) \
              + " TABLE: " + repr (self.constant_set.type_table)\
              + " RUN RANGE: " + repr(self.run_range)\
              + " VARIATION: " + repr(self.variation)\
              + " SET: " + repr (self.constant_set)
        print "      |"
        print "      +-->" + repr(self.constant_set.vault)
        print "      +-->" + repr(self.constant_set.data_list)
        print "      +-->" + repr(self.constant_set.data_table)



class RunRange(Base):
    __tablename__ = 'runRanges'
    id = Column(Integer, primary_key=True)
    name = Column(String)
    created = Column(DateTime, default = datetime.datetime.now)
    modified = Column(DateTime, default = datetime.datetime.now, onupdate = datetime.datetime.now)
    comment = Column(Text)
    min = Column('runMin',Integer)
    max = Column('runMax',Integer)

    def __repr__(self):
        if self.name != "":
            return "<RunRange {0} {3}:{1}-{2}>".format(self.id, self.min, self.max, self.name)
        else:
            return "<RunRange {0} '{1}-{2}'>".format(self.id, self.min, self.max)



class Variation(Base):
    __tablename__ = 'variations'
    id = Column(Integer, primary_key=True)
    name = Column(String)
    created = Column(DateTime, default = datetime.datetime.now)
    modified = Column(DateTime, default = datetime.datetime.now, onupdate = datetime.datetime.now)
    comment = Column(Text)

    def __repr__(self):
        return "<Variation {0} '{1}'>".format(self.id, self.name)

#--------------------------------------------
# flattens arrays of arrays to one array
#--------------------------------------------
def gen_flatten_data(data):
    """
    get generator that flattens 'arrays of arrays' to one array

    :param data: List which probably contains sub-collections
    :type data: []
    :return: flattened list
    :rtype: generator

    example
    >>>list(gen_flatten_data([[[1, 2, 3], [4, 5]], "abs"]))
    [1, 2, 3, 4, 5, "abs"]

    """
    for el in data:
        if isinstance(el, collections.Iterable) and not isinstance(el, basestring):
            for sub in gen_flatten_data(el):
                yield sub
        else:
            yield el


#--------------------------------------------
# flattens arrays of arrays to one array
#--------------------------------------------
def flatten_data(data):
    """
     flattens arrays of arrays to one array

    :param data: List which probably contains sub-collections
    :type data: []
    :return: flattened list
    :rtype: generator

    example
    >>>flatten_data([[[1, 2, 3], [4, 5]], "abs"])
    [1, 2, 3, 4, 5, "abs"]

    """
    return list(gen_flatten_data(data))


#--------------------------------------------
# Get tabled data, convert it to string blob for db insertion
#--------------------------------------------
def list_to_blob(data):
    """
    Get tabled data, convert it to string blob for db insertion

    if you have tabled data use gen_flatten_data to flatten data first


    :param data: FLATTENED list of values
    :type data: []
    :return: string with text-blob for database insertion
    :rtype: str

    >>>list_to_blob([1,"2","str"])
    "1|2|str"
    >>>list_to_blob(["strings", "with|surprise"])
    "strings|with&delimiter;surprise"
    """
    def prepare_item(item):
        if not isinstance(item, basestring):
            str = repr(item)
        else:
            str = item
        return str.replace(blob_delimiter, blob_delimiter_replacement)

    if len(data) == 0: return ""
    if len(data) == 1: return prepare_item(data[0])

    #this for data[:-1] makes result like a1|a2|a3
    blob = ""
    for item in data[:-1]:
        blob += prepare_item(item) + blob_delimiter
    blob+=prepare_item(data[-1])

    return blob

#--------------------------------------------
# Get blob data and convert it to list decoding blob_delimiter
#--------------------------------------------
def blob_to_list(blob):
    """
    Get blob data and convert it to list decoding blob_delimiter

    :param blob:
    :type blob: str
    :return:

    >>>blob_to_list("1|2|str")
    ["1","2","str"]

    >>>blob_to_list("strings|with&delimiter;surprise")
    ["strings", "with|surprise"]
    """
    splits = blob.split(blob_delimiter)
    items = []
    for item in splits:
        items.append(item.replace(blob_delimiter_replacement, blob_delimiter))
    return items

#--------------------------------------------
# Converts flat array to tabled array
#--------------------------------------------

def list_to_table(data, col_count):
    """
    Converts flat array to tabled array

    :param data: flat list with data
    :type data: []
    :param col_count:number of columns
    :type col_count: int
    :return: tabled data
    :rtype:[]


    >>> list_to_table([1,2,3,4,5,6], 3)
    [[1,2,3],[4,5,6]]
    """

    if len(data) % col_count != 0:
        message = "Cannot convert list to table."\
                + "The total number of cells ({0}) is not compatible with the number of columns ({1})"\
                .format(len(data), col_count)
        raise ValueError(message)

    row_count = len(data) / col_count
    #cpp way
    table = []
    for row_i in range(row_count):
        row = []
        for col_i in range(col_count): row.append(data[row_i*col_count + col_i])
        table.append(row)
    return table








def decode_data(data):
    pass


if __name__=="__main__":
    root_dir = Directory()
    root_dir.path = '/'
    root_dir.name = ''
    root_dir.id = 0

    engine = sqlalchemy.create_engine('mysql://ccdb_user@127.0.0.1/ccdb')

    def structure_dirs(dirs):
        """

        @type dirs: {} dictionary with dir.id as a key
        """
        assert(isinstance(dirs,type({})))

        #clear the full path dictionary
        dirsByFullPath = {root_dir.path: root_dir}

        #begin loop through the directories
        for dir in dirs.values():
            assert (isinstance(dir, Directory))

            parent_dir = root_dir

            # and check if it have parent directory
            if dir.parent_id >0:
                #this directory must have a parent! so now search it
                parent_dir = dirs[dir.parent_id]

            parent_dir.sub_dirs.append(dir)
            dir.path = posixpath.join(parent_dir.path, dir.name)
            dir.parent_dir = parent_dir
            dirsByFullPath[dir.path] = dir

        return dirsByFullPath
        #end of structure_dirs()


    def get_dirs_by_id_dic(dirs):
        result = {}
        for dir in dirs:
            assert(isinstance(dir, Directory))
            result[dir.id]=dir

        return result


    Session = sessionmaker(bind=engine)
    session = Session()
    dirsById = get_dirs_by_id_dic(session.query(Directory))
    dirsByFullPath = structure_dirs(dirsById)


    for key,val in dirsByFullPath.items():
            print key, val, val.id

    experiment_dir = dirsByFullPath['/test/test_vars']

    assert (isinstance(experiment_dir, Directory))

    t = TypeTable()

    for table in experiment_dir.type_tables:
        print " TABLE: " + table.name
        print " +--> COLUMNS:"

        for column in table.columns:
            print "      +-->" + column.name

        print " +--> CONSTANTS:"

        for set in table.constant_sets:
            print "      +-->" + set.vault


    #assignments = session.query(Assignment)

    #for assignment in assignments:
     #   assignment.print_deps()

    query = session.query(Assignment).join(ConstantSet).join(TypeTable).join(RunRange).join(Variation)\
            .filter(Variation.name == "default").filter(TypeTable.name=="test_table").filter(RunRange.min<=1000).filter(RunRange.max>=1000)\
            .order_by(desc(Assignment.id)).limit(1).one()

    print query

    print query.print_deps()

    #for assignment in query:
    #    assignment.print_deps()
    print session.dirty

    q = session.query(Directory)
    q = q.limit(1)

    print q